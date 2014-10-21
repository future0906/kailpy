//=============================================================================
//.______   ____    ____  __  ___      ___       __   __      
//|   _  \  \   \  /   / |  |/  /     /   \     |  | |  |     
//|  |_)  |  \   \/   /  |  '  /     /  ^  \    |  | |  |     
//|   ___/    \_    _/   |    <     /  /_\  \   |  | |  |     
//|  |          |  |     |  .  \   /  _____  \  |  | |  `----.
//| _|          |__|     |__|\__\ /__/     \__\ |__| |_______|
//                                                            
//=============================================================================
#ifndef __H_COMMON_H_
#define __H_COMMON_H_

#include<stdio.h>
#include<time.h>

#define NULL 0
#define PORT_ANY 0
#define MAX_MAIN_BUFFER 64
//Max udp buffer ,notice this is relative to how man message in one packet
#define MAX_UDP_BUFFER 4096
#define USER_NAME_LENGTH 32
#define APP_NAME_LENGTH 128
#define CACHE_LENGTH 256
#define OUT_CACHE_LENGTH 256
#define DEFAULT_IPM 3
#define MAIN_PORT 27888
#define CONSOLE_PORT 27999
#define MAX_PLAYER 256

#ifdef _DEBUG
#define DEBUG//Enable debug
#define MEMORY_LEAK//Memory leak detecting(Win32)
#endif

#ifdef WIN32
typedef int socklen_t;
#define snprintf _snprintf
#endif

void _MyAssert(char* , unsigned);

#ifdef DEBUG
#define __ASSERT(f) \
if(f) \
	NULL; \
else \
	_MyAssert(__FILE__ , __LINE__)
#else
#define __ASSERT(f) NULL
#endif

#ifdef DEBUG
#define __PRINT(F,X) printf(F,X)
#else
#define __PRINT(F,F) NULL
#endif

//Compare
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifndef WIN32
//TODO:Implements GetTickCount

#endif
#endif//__H_COMMON_H_
