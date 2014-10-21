#include<stdio.h>
#include "common.h"
#include "game.h"
#include "user.h"
#include "klist.h"
#include "connection.h"
#include<signal.h>

//============Memory leak detect==================
#ifdef MEMORY_LEAK
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_FILE_STDCOUT
#include<stdlib.h>
#include<crtdbg.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

bool Running = true;

//===================Signal handler================
void TerminalHandler(int signal)
{
    __PRINT("Got a terminal signal, stopping\n", NULL);
    Running = false;
}
//===================Main function=================
int main(int argc, char* argv)
{
	int buf_len;
	char main_buffer[MAX_MAIN_BUFFER];
	KConnectionManager::Initialize();
    KConnectionManager km = KConnectionManager();
	KConnection* main_con = km.create_con(MAIN_PORT);
	KLobby lobby = KLobby(&km);
    //Handle signal
    signal(SIGINT, TerminalHandler);
    //Debug memory leak
#ifdef MEMORY_LEAK
    _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_WNDW );
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif//MEMORY_LEAK

	sockaddr remote_addr;
	while(Running) {//MainLoop
 		km.check(1,0);
	    if (main_con->has_data()) {
			//Reinitialize buffer and len
			buf_len = MAX_MAIN_BUFFER;
			memset(main_buffer, 0, sizeof(main_buffer));
 			memset(&remote_addr, 0, sizeof(remote_addr));	
			//Recv data
			buf_len = main_con->recv_data(main_buffer, buf_len, false);
			main_con->get_raddr(&remote_addr);
			if (strncmp("PING", main_buffer, MAX_MAIN_BUFFER) == 0) {//Echo message
				//TODO:Send "PONG"
			}
			else if (strncmp("HELLO", main_buffer, 5) == 0) {
				if (strncmp("0.83", main_buffer + 5, MAX_MAIN_BUFFER - 5) == 0) {
					//XXX:Rember to delete user and game at list
					KUser* nuser = lobby.new_user();
					__PRINT("Port:%d\n", nuser->get_port());
					snprintf(main_buffer, MAX_MAIN_BUFFER, "HELLOD00D%i", nuser->get_port());
					main_con->send(main_buffer,sizeof(main_buffer));
				}
				else {
					//Version error
					//TODO:Send "VER"
				}
			}
			else {
				printf("Got an unexcept data:%s", main_buffer);		
			}
			__PRINT("Recvdata:%s\n", main_buffer);		
		}
	lobby.tick();
	}
    km.Release();
	return 0;
}
