//=============================================================================
//.______   ____    ____  __  ___      ___       __   __      
//|   _  \  \   \  /   / |  |/  /     /   \     |  | |  |     
//|  |_)  |  \   \/   /  |  '  /     /  ^  \    |  | |  |     
//|   ___/    \_    _/   |    <     /  /_\  \   |  | |  |     
//|  |          |  |     |  .  \   /  _____  \  |  | |  `----.
//| _|          |__|     |__|\__\ /__/     \__\ |__| |_______|
//                                                            
//=============================================================================
#ifndef __H_CONNECTION_H_
#define __H_CONNECTION_H_
#include <iostream>
#include "instruction.h"
#include "common.h"
#include "klist.h"

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef __FreeBSD__
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

using std::cout;

class KConnection
{
public:
	enum ConnectionType{TCP = 1, UDP = 2};

private:
	bool m_havedata;
	int m_sock;
	int m_port;
	ConnectionType m_type;
	sockaddr m_raddr;
public:
	KConnection(int, ConnectionType);
	KConnection(int);
	KConnection* Accept();
    virtual ~KConnection(){}
	virtual int get_port();
	int get_fd();
	void set_dataflag(bool);
	bool has_data();
	int recv_data(char*, int, bool);
	void send(char*, int);
	int get_raddr(sockaddr*);
};

class KKailConnection : public KConnection
{
private:
	int m_last_serial;//the latest handled instruction
	int m_last_send;//the latest send instruction
	int m_cache_len;//cache length 
	int m_outcache_len;
	int m_ipm;//Instruction Per Message
	KInstruction* m_cache[CACHE_LENGTH];//Recive instruction cache
	KInstruction* m_outcache[OUT_CACHE_LENGTH];
public: 
	KKailConnection(int, int);
    virtual ~KKailConnection();
	KPacket* getPacket();
	KInstruction* getLastInstruction();
	void sendInstruction(KInstruction*);
	void deletePacket(KPacket*);
	int getIPM();
	bool has_cache();
};

class KConnectionManager
{
private:
	KList<KConnection*> m_conlist;		
	fd_set mp_allfds;
	int mp_nfds;
public:
	KConnectionManager();
    int Release();
	KConnection* create_con(int);
	KConnection* create_con_by_fd(int);

	KKailConnection& create_kcon(int);
	static void Initialize();
	void check(int,int);
};

#endif//__H_CONNECTION_H_

