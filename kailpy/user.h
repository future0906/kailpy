#ifndef __H_USER_H_
#define __H_USER_H_
#include "connection.h"
typedef enum CONNECTION_TYPE {
	EMPT,LAN,EXC,GOOD,AVG,LOW,BAD
};

class KLobby;
class KUser {
private:
	unsigned short m_id;
	unsigned int m_ping;//delay count
	unsigned char m_status;//user status
    unsigned int m_timeout;
    unsigned int m_total_pingtime;
    unsigned int m_ping_sendtime;
	int m_ping_count;
	CONNECTION_TYPE m_type : 8;
	char m_appname[APP_NAME_LENGTH];
	char m_username[USER_NAME_LENGTH];
	KKailConnection* mp_kcon;
public:
	KUser(KKailConnection*);
	~KUser();
	int get_port();
	void tick(KLobby*);
    int toData(unsigned char*, int*);
    int sendInstruction(KInstruction* inst);

    void SetID(unsigned short id) { m_id = id;}
    unsigned short GetID() { return m_id;}

public:
    unsigned char get_status() { return m_status; }
};
#endif

