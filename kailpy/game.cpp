#include "common.h"
#include "game.h"
///////////////////////////////////////////////////////////////////
//KLobby method
KLobby::KLobby(KConnectionManager* km)
{
	mp_km = km;
    m_connecting_count = 0;
    m_curid = 0;
    //m_last_check_time = 0;
}

KUser* KLobby::new_user()
{
	KKailConnection& p_kcon = mp_km->create_kcon(PORT_ANY);	
	KUser* p_user = new KUser(&p_kcon);
    p_user->SetID(m_curid++);
	m_userlist.addData(p_user);
    m_connecting_count++;
	return p_user;
}

int KLobby::sendAll(KInstruction *inst)
{
    KListNode<KUser*>* i_node;
    KList<KUser*>::iterator it = m_userlist.begin();
    int rtn = 1;
    while (i_node = it.next()) {
        rtn = i_node->m_data->sendInstruction(inst);
        //Send failed, return directly
        if (rtn == 0) {
            break;
        }
    }
    return rtn;
}
void KLobby::tick()
{
	KListNode<KUser*>* i_node;
	KList<KUser*>::iterator it = m_userlist.begin();

	while (i_node = it.next()) {
		i_node->m_data->tick(this);
	}
}
/*
    根据ID寻找用户
*/
KUser* KLobby::findUserByID(unsigned short id)
{
    KList<KUser*>::iterator it = m_userlist.begin();
    KUser* result = NULL;
    KListNode<KUser*>* p_node;
    while ((p_node = it.next()) !=0) {
        result = p_node->m_data;
        if (result->GetID() == id) {
            break;
        }
    }
    return result;
}
/*
    删除用户
*/
int KLobby::removeUserByID(unsigned short id)
{
    KList<KUser*>::iterator it = m_userlist.begin();
    KListNode<KUser*>* p_node;
    while ((p_node = it.next()) !=0) {
        if (p_node->m_data->GetID() == id) {
            m_userlist.remove(it);
            return true;
        }
    }
    return false;
}
/*
    Get all user data in current lobby
*/
int KLobby::get_UData(unsigned char* buffer, int* len)
{
	unsigned char* result;
	KUser* cur_user;
    KListNode<KUser*>* p_node;
	int ptr = 0;//pointer
	if (buffer == NULL) {
        __PRINT("[KLoby::get_UData] Buffer ptr error", NULL);
        return 0;
	}
	else {
		result = buffer;
	}
	KList<KUser*>::iterator it = m_userlist.begin();
	while ((p_node = it.next()) != 0) {
		int dl = 0;//data len
        cur_user = p_node->m_data;
        if (cur_user->get_status() != 2) {//not connecting
		    cur_user->toData(result + ptr, &dl);
		    ptr += dl;
        }
	}
	*len = ptr;
	return 1;
}
KLobby::~KLobby()
{
    KListNode<KUser*>* p_user_cur;
    KList<KUser*>::iterator it = m_userlist.begin();
    while ((p_user_cur = it.next()) != NULL) {
        delete p_user_cur->m_data;
    }

    KListNode<KGame*>* p_game_cur;
    KList<KGame*>::iterator it2 = m_gamelist.begin();
    while ((p_game_cur = it2.next()) != NULL) {
        delete p_game_cur->m_data;
    }
}