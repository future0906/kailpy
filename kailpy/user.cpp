#include "game.h"
#include "instruction.h"
#include "user.h"
#include "common.h"

KUser::KUser(KKailConnection* kcon)
{
	mp_kcon = kcon;	
	m_ping_count = 0;
    m_ping = 100;
	memset(m_appname, 0, APP_NAME_LENGTH);
	memset(m_username, 0, USER_NAME_LENGTH);
	m_type = EMPT;
    m_status = 2;//0=Playing, 1=Idle, 2=Connecting
    m_total_pingtime = 0;
    m_ping_sendtime = 0;
}

int KUser::get_port()
{
	return mp_kcon->get_port();
}

int KUser::sendInstruction(KInstruction *inst)
{
    mp_kcon->sendInstruction(inst);
    return 1;
}

void KUser::tick(KLobby* lobby)
{
	if (mp_kcon->has_data() || mp_kcon->has_cache()) {
		KInstruction* last_inst;
		last_inst = mp_kcon->getLastInstruction();
        if (last_inst == NULL) {
            return;
        }
		last_inst->print();
    	switch(last_inst->getId())
		{
			//user login
			case USERLOGN:
			{
				//TODO:calculate user's ping
				KInstruction resp_inst;
				strncpy(m_username, last_inst->getUsername(), sizeof(m_username));
				last_inst->getString(m_appname, sizeof(m_appname));
				m_type = (CONNECTION_TYPE)last_inst->getChar();
				resp_inst.init_body(16);
				resp_inst.setId(SERVPING);
				resp_inst.setInt(0);
				resp_inst.setInt(1);
				resp_inst.setInt(2);
				resp_inst.setInt(3);
				mp_kcon->sendInstruction(&resp_inst);
                //First login tick
                m_ping_sendtime = GetTickCount();
				break;
			}
			//XXX:Server ping and user pong is used to let client know
			//	 how man instrucionts per message,
			//   basicly, send IPM + 1 server ping 
			case USERPONG:
			{
				if (m_ping_count < mp_kcon->getIPM()) {
                    
                    unsigned int now = GetTickCount();
                    m_total_pingtime += (now - m_ping_sendtime);
                    std::cout << m_total_pingtime<<","<<now<<","<<m_ping_sendtime<<std::endl;
                    m_ping_sendtime = now;
					//发送新的PING
                    KInstruction resp_inst;
					resp_inst.init_body(16);
					resp_inst.setId(SERVPING);
					resp_inst.setInt(0);
					resp_inst.setInt(1);
					resp_inst.setInt(2);
					resp_inst.setInt(3);
					mp_kcon->sendInstruction(&resp_inst);
					m_ping_count++;
					__PRINT("Got user pong\n", NULL);
				}
				else {//ipm sync complete
                    m_ping = m_total_pingtime / m_ping_count;
                    //Sever Info
					KInstruction resp_inst;
                    int u_buff_size = (lobby->getUserCount() - lobby->getConnectingCount()) * (USER_NAME_LENGTH + 8);
                    
					resp_inst.setId(LONGSUCC);
					resp_inst.setUsername("", 0);
					resp_inst.init_body(u_buff_size + 8);
					resp_inst.setInt(lobby->getUserCount() - lobby->getConnectingCount());
                    resp_inst.setInt(lobby->getGameCount());
                    //Here may seem strange, but can avoid some memory copy
                    //Cause user data's length is hard to get, so I first allocate max
                    //buffer may be use, an then change body size to actual size.
                    lobby->get_UData(resp_inst.getBody() + 8, &u_buff_size);
                    resp_inst.setBodyLen(u_buff_size + 8);

					mp_kcon->sendInstruction(&resp_inst);
                    //Confirm login success
					resp_inst.setId(USERJOIN);
					resp_inst.setUsername(m_username, strlen(m_username));
					resp_inst.init_body(7);
					resp_inst.setShort(m_id);
					resp_inst.setInt(m_ping);
					resp_inst.setChar((char)m_type);
					lobby->sendAll(&resp_inst);
                    //Message of today
                    resp_inst.setId(MOTDLINE);
                    resp_inst.setBody((unsigned char*)"Test", 5);
					mp_kcon->sendInstruction(&resp_inst);
                    //Set status as connected
                    m_status = 1;//Idle
                    lobby->minusConnecting();
				}
                break;
			}
            case PARTCHAT:
            {
                //TODO:May be some chat filter here
                last_inst->setUsername(m_username, strlen(m_username));
                lobby->sendAll(last_inst);
                break;
            }
            case USERLEAV:
            {
                if (last_inst->getShort() == 0xFFFF) {//Userid match
                    lobby->removeUserByID(m_id);
                }
                last_inst->resetIP();
                last_inst->setShort(m_id);
                lobby->sendAll(last_inst);
            }
            case TMOUTRST:
            {
                m_timeout = 0;//清零超时位
            }
		}		
	delete last_inst;
	}
}

//this function accept a int pointer as argument,
//len as buffer size for input and output as a result len
int KUser::toData(unsigned char* buf, int* len)
{
    unsigned char* result;
    int ptr = 0;
    if (buf == 0) {
        __PRINT("[ToData] Buf ptr error\n", NULL);
        return FALSE;
    }
    else {
        result = buf;
    }
    strncpy((char*)result, m_username, sizeof(m_username));
    ptr += (strlen(m_username) + 1);

	*(unsigned int*)(result + ptr) = m_ping;
	ptr += 4;
	*(unsigned char*)(result + ptr) = m_status;	
	++ptr;
	*(unsigned short*)(result + ptr) = m_id;	
	ptr += 2;
	*(CONNECTION_TYPE*)(result + ptr) = m_type;	
	++ptr;
	//return len
	*len = ptr;
	return TRUE;
}

KUser::~KUser()
{

}
