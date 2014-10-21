#include "common.h"
#include "connection.h"
//Construct function
KConnection::KConnection(int port, ConnectionType type)
{
	int sock_len;
	sockaddr_in saddr;
	sockaddr_in tempaddr;
	if (type == KConnection::UDP) {
		this->m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	else {
		this->m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	if (this->m_sock < 0) {
#ifdef WIN32
		cout<<"Create socket error:" << WSAGetLastError();
		//Win32
		WSACleanup();
#endif
#ifdef __FreeBSD__
		perror("Create socket error");
#endif
		exit(-1);
	}
	//socket type
	m_type = type;

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
#ifdef WIN32
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
#endif

#ifdef __FreeBSD__
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

	if (bind(this->m_sock, (sockaddr*)(&saddr), sizeof(saddr)) < 0) {
#ifdef WIN32
		cout<<"Unable to bind:" << WSAGetLastError();
		//Win32
		WSACleanup();
#endif
#ifdef __FreeBSD__
		perror("Unable to bind");
#endif
		exit(-1);
	}
	//Start listen
	listen(this->m_sock, SOMAXCONN);
	if (port == 0) {
		sock_len = sizeof(tempaddr);
		memset(&tempaddr, 0, sock_len);
		getsockname(m_sock, (sockaddr*)&tempaddr, (socklen_t*)&sock_len);
		this->m_port = ntohs(tempaddr.sin_port);
	}
	else {
		this->m_port = port;
	}
	//Initialize other data
	m_havedata = false;
	memset((void*)&m_raddr, 0, sizeof(sockaddr));
	__PRINT("Listening on %d........\n", this->m_port);
}
//Construct a KConnection object direct from FileDescriptor
KConnection::KConnection(int fd)
{
	m_sock = fd;
	sockaddr_in tempaddr;
	int addr_len = sizeof(sockaddr);
	memset(&tempaddr, 0, addr_len);
	getsockname(m_sock, (sockaddr*)&tempaddr, &addr_len);
	this->m_port = tempaddr.sin_port;
}

int KConnection::get_port()
{
	return m_port;
}

int KConnection::get_fd()
{
	return m_sock;
}

void KConnection::set_dataflag(bool flag = true)
{
	m_havedata = true;
}

bool KConnection::has_data()
{
	return m_havedata;
}

KConnection* KConnection::Accept()
{
	if (m_type != KConnection::TCP) {
		return NULL;
	}
	//accept(m_sock, 
}

int KConnection::recv_data(char* buffer, int buf_len, bool is_peek)
{
	int data_len;
	sockaddr remote_addr;
	int ralen = sizeof(remote_addr);
	m_havedata = false;
	if (data_len = recvfrom(m_sock,
							buffer,
							buf_len,
							is_peek ? MSG_PEEK : 0,
							&remote_addr, (socklen_t*)&ralen) <= 0) {
		//error
		return -1;
	} 
	else {
		//No errors, copy src address
		memcpy(&m_raddr, &remote_addr, sizeof(m_raddr));
		return data_len;
	}
}

void KConnection::send(char* buffer, int buf_len)
{
	sendto(m_sock, buffer, buf_len, 0, &m_raddr, sizeof(m_raddr));
}

int KConnection::get_raddr(sockaddr* p_raddr)
{
	memcpy(p_raddr, &m_raddr, sizeof(m_raddr));
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////
//KailConnection function
KKailConnection::KKailConnection(int port, int ipm = DEFAULT_IPM):
				KConnection(port, KConnection::UDP)
{
	m_last_serial = 0;
	m_last_send = 0;
	m_cache_len = 0;
	m_outcache_len = 0;
	m_ipm = ipm;
}

//Parse receive to packet
KPacket* KKailConnection::getPacket()
{
	char buffer[MAX_UDP_BUFFER];
	int ptr = 0;

	KInstruction* instruction_array;

	memset(buffer, 0, MAX_UDP_BUFFER);
	KPacket* p_packet = new KPacket();
	KConnection::recv_data(buffer, MAX_UDP_BUFFER, false);
	p_packet->m_count = *buffer;
	ptr += 1;
	//allocate instruction array
	instruction_array = new KInstruction[p_packet->m_count];
	p_packet->mp_instructions = instruction_array;
	for (int i = 0;i < p_packet->m_count; i++) {
		KInstruction* cur_inst;
		int user_name_len = 0;
		int body_len = 0;

		cur_inst = instruction_array + i;
		cur_inst->setSerial(*(unsigned short*)(buffer + ptr));
		ptr +=2;
		cur_inst->setLength(*(unsigned short*)(buffer + ptr));
		ptr +=2;
		cur_inst->setId(*(INSTRUCTION*)(buffer + ptr));
		ptr +=1;
		//username section
		user_name_len = strlen(buffer + ptr);	
		if (user_name_len > 0){//have user name
			cur_inst->setUsername(buffer + ptr, user_name_len);
			ptr = ptr + user_name_len;
		}
		ptr +=1;

		//copy remain body
		body_len = cur_inst->getLength() - 2 - user_name_len;
		
		if (cur_inst->setBody((unsigned char*)buffer + ptr, body_len) < 0) {
			__PRINT("Error:Out of memory\n", NULL);
			exit(-1);
		}
		ptr += body_len;
	} 
	return p_packet;
}

//send one instruction
//
//TODO:split into different function if needed
//XXX:This function use array as a cache structure, and it's tooo complicate,
//    rewrite this was really needed
void KKailConnection::sendInstruction(KInstruction* p_inst)
{
	int total_inst;
	int byte_count;
	unsigned char out_buf[MAX_UDP_BUFFER];
	KInstruction* p_cinst = new KInstruction(*p_inst);

	//1,If out_cache full,remove head.
	//2.move cache forward 
	//3.make new inst at last
	p_cinst->setSerial(m_last_send++);
	if (m_outcache_len >= OUT_CACHE_LENGTH) {//1
		--m_outcache_len;
		delete m_outcache[0];
		memcpy(m_outcache, m_outcache + 1, sizeof(m_outcache) - 1);//2
	}
	m_outcache[m_outcache_len] = p_cinst;//3
	++m_outcache_len;

	//start reading at tail,because tail will be the newest
	total_inst = MIN(m_ipm, m_outcache_len);
	byte_count = 0;

	*(out_buf) = (unsigned char) total_inst;
	byte_count++;

	for (int i = m_outcache_len - 1;i > m_outcache_len - 1 - total_inst;i--) {
		m_outcache[i]->toData(out_buf + byte_count, sizeof(out_buf) - byte_count);
		byte_count += m_outcache[i]->getLength() + 4;//include serial and length it self
	}
	send((char*)out_buf, byte_count);
}
//find last instruction
KInstruction* KKailConnection::getLastInstruction()
{
	if (m_cache_len <= 0) {//No cache
		int i;
		bool inst_missing_flag = false;
		KPacket* p_packet = getPacket();
		KInstruction* p_first_inst = NULL;
		for (i = p_packet->m_count - 1;i >= 0;i--) {
			KInstruction* cur_inst = NULL;
			KInstruction* in_cache_inst = NULL;

			cur_inst = p_packet->mp_instructions + i;
			if (cur_inst->getSerial() < m_last_serial) {//handled instruction,ignore it
				continue;
			}
			//copy instruction
			in_cache_inst = new KInstruction(*cur_inst);
			if (p_first_inst == 0) {//First instruction do not need to put in cache
				p_first_inst = in_cache_inst;
				continue;
			}
			//save other in cache
			//insert into tail
			m_cache[m_cache_len++] = in_cache_inst;
		}
		deletePacket(p_packet);
		++m_last_serial;
		return p_first_inst;
		//delete packet
	}
	else {//got cache
		KInstruction* in_cache_inst = NULL;
		in_cache_inst = m_cache[0];
		--m_cache_len;
		memcpy(m_cache, m_cache + 1, m_cache_len * sizeof(KInstruction*)); 
		return in_cache_inst;
	}
}
bool KKailConnection::has_cache()
{
	return m_cache_len > 0;
}
//return instruction per message
int KKailConnection::getIPM()
{
	return m_ipm;
}
//delete packet
void KKailConnection::deletePacket(KPacket* p_packet)
{
	delete[] p_packet->mp_instructions;	
	delete p_packet;
}
//Destruction
KKailConnection::~KKailConnection()
{
    for (int i = 0; i < m_cache_len; i++) {
        delete m_cache[i];
    }
    for (int i = 0; i < m_outcache_len; i++) {
        delete m_outcache[i];
    }
}
/////////////////////////////////////////////////////////////////////////////////
//Connection Manager function
KConnectionManager::KConnectionManager()
{
	FD_ZERO(&mp_allfds);
	mp_nfds = 0;	
}

KConnection* KConnectionManager::create_con(int port)
{
	KConnection* p_con;
	int new_fd;
	p_con = new KConnection(port, KConnection::UDP);
	new_fd = p_con->get_fd();
	FD_SET(new_fd, &this->mp_allfds);
	this->m_conlist.addData(p_con);
	if (new_fd > mp_nfds) {//find bigest fd
		mp_nfds = new_fd;
	}
	return p_con;
}

KConnection* KConnectionManager::create_con_by_fd(int fd)
{
	KConnection* p_con;
	p_con = new KConnection(fd);
	FD_SET(fd, &this->mp_allfds);
	this->m_conlist.addData(p_con);
	if (fd > mp_nfds) {//find bigest fd
		mp_nfds = fd;
	}
	return p_con;
}

KKailConnection& KConnectionManager::create_kcon(int port)
{
	KKailConnection* p_kcon;
	int new_fd;
	p_kcon = new KKailConnection(port);
	new_fd = p_kcon->get_fd();
	FD_SET(new_fd, &this->mp_allfds);
	this->m_conlist.addData(p_kcon);
	if (new_fd > mp_nfds) {//find bigest fd
		mp_nfds = new_fd;
	}
	return *p_kcon;
}

void KConnectionManager::check(int secs, int ms)
{
	int tmp_fd;
	KConnection* tmp_con;
	KListNode<KConnection*>* i_node;
	timeval sleep_time;
	fd_set temp;

	sleep_time.tv_sec = secs;
	sleep_time.tv_usec = ms;
	memcpy(&temp, &mp_allfds, sizeof(temp));
	//check all connection wheather have data
	select(mp_nfds + 1, &temp, NULL, 0, &sleep_time);
    
    KList<KConnection*>::iterator it = m_conlist.begin();

	while (i_node = it.next()) {
		tmp_con = i_node -> m_data;
		tmp_fd = tmp_con -> get_fd();
		if (FD_ISSET(tmp_fd, &temp)) {//have data 
			tmp_con->set_dataflag();
		} 
	}
}

int KConnectionManager::Release()
{
    KListNode<KConnection*>* p_cur;
    KList<KConnection*>::iterator it = m_conlist.begin();
    while ((p_cur = it.next()) != NULL) {
        delete p_cur->m_data;
    }
    return 1;
}

//Initialize function(Only useful in win32)
void KConnectionManager::Initialize()
{
#ifdef WIN32
	WSADATA wsd;
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0)
	{
		cout<<"WSA Initialization failed!\r\n";
		WSACleanup();
	}
#endif
}

