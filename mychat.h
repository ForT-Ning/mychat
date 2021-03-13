#ifndef _MY_CHAT
#define _MY_CHAT

#include <iostream>
#include <mqueue.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

using namespace std;

#define SA struct sockaddr
#define SERVERPORT 12356
#define MAXLINE 1024

#endif //_MY_CHAT