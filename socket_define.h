//
// Created by csw on 2021/7/9.
// email: cswxzx@gmail.com
//

#ifndef PROJECT_SOCKET_DEFINE_H
#define PROJECT_SOCKET_DEFINE_H

#if defined(__unix__)
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
typedef int SOCKET;
typedef unsigned int sockaddr_len_t;
#define closesocket(s) close(s)
#else
typedef int sockaddr_len_t;
#endif

#endif //PROJECT_SOCKET_DEFINE_H
