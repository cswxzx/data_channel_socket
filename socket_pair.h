//
// Created by csw on 2021/7/8.
// email: cswxzx@gmail.com
//


#ifndef PROJECT_SOCKET_PAIR_H
#define PROJECT_SOCKET_PAIR_H

#include "stdafx.h"
#include "socket_define.h"

class SocketPair {
private:
    SocketPair();
public:
    ~SocketPair();
public:
    static std::unique_ptr<SocketPair>& instance();
    static void release();
public:
    uint64_t create_pair(SOCKET fd[2]);
    void delete_pair(uint64_t handle);
    void clear();
private:
    struct _impl_t;
    _impl_t *m_impl;
};

#endif /* PROJECT_SOCKET_PAIR_H */
