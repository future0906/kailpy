#ifndef __H_GAME_H_
#define __H_GAME_H_
#include "klist.h"
#include "connection.h"
#include "instruction.h"
#include "user.h"

////////////////////////////////////////////////////////////
//KGame class
class KGame {
	
};

///////////////////////////////////////////////////////////
//KLobby class
class KLobby {
private:
	KList<KUser*> m_userlist;
	KList<KGame*> m_gamelist;
	KConnectionManager* mp_km;
    int m_connecting_count;
    int m_curid;
public:
	KLobby(KConnectionManager*);
    int minusConnecting() { return --m_connecting_count; }
    int getConnectingCount() { return m_connecting_count; }
    int getUserCount() { return m_userlist.getLength(); }
    int getGameCount() { return m_gamelist.getLength(); }
    KUser* findUserByID(unsigned short id);
    int removeUserByID(unsigned short id);
	KUser* new_user();
    int sendAll(KInstruction* inst);
	void tick();
	int get_UData(unsigned char*, int*);
    ~KLobby();
};
#endif

