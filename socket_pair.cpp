//
// Created by csw on 2021/7/8.
// email: cswxzx@gmail.com
//

#include "socket_pair.h"

struct SocketPair::_impl_t {
    _impl_t() : m_listener(-1) {
    }

    ~_impl_t() {
        closesocket(m_listener);
        m_pairs.clear();
    }

    uint64_t create_pair2(int family, int type, int protocol, SOCKET fd[2]) {
        SOCKET connector = -1;
        SOCKET acceptor = -1;
        
        struct sockaddr_in connect_addr;
        sockaddr_len_t size = 0;
        int saved_errno = -1;
        _pair_type_ptr pp;
        uint64_t handle;

        if (!fd) {
            return 0;
        }

        if (m_listener == -1) {
            SOCKET listener = socket(AF_INET, type, protocol);
            if (listener < 0) {
                return 0;
            }

            memset(&m_listen_addr, 0, sizeof(m_listen_addr));
            m_listen_addr.sin_family = AF_INET;
            m_listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            m_listen_addr.sin_port = 0;
            if (bind(listener, (struct sockaddr*)&m_listen_addr, sizeof(m_listen_addr)) == -1) {
                goto tidy_up_and_fail;
            }

            if (listen(listener, 1) == -1) {
                goto tidy_up_and_fail;
            }

            m_listener = listener;
        }

        connector = socket(family, type, protocol);
        if (connector < 0) {
            goto tidy_up_and_fail;
        }

        size = sizeof(connect_addr);
        if (getsockname(m_listener, (struct sockaddr*)&connect_addr, &size) == -1) {
            goto tidy_up_and_fail;
        }

        if (size != sizeof(connect_addr)) {
            goto tidy_up_and_fail;
        }

        if (connect(connector, (struct sockaddr*)&connect_addr, sizeof(connect_addr)) == -1) {
            goto tidy_up_and_fail;
        }

        acceptor = accept(m_listener, (struct sockaddr*)&m_listen_addr, &size);
        if (acceptor < 0) {
            goto tidy_up_and_fail;
        }

        if (size != sizeof(m_listen_addr)) {
            goto tidy_up_and_fail;
        }

        if (getsockname(connector, (struct sockaddr*)&connect_addr, &size) == -1) {
            goto tidy_up_and_fail;
        }

        if (size != sizeof(connect_addr) 
            || m_listen_addr.sin_family != connect_addr.sin_family
            || m_listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
            || m_listen_addr.sin_port != connect_addr.sin_port) {
                goto tidy_up_and_fail;
        }

        pp = std::make_unique<_pair_type>();
        pp->m_fd[0] = connector;
        pp->m_fd[1] = acceptor;
        fd[0] = pp->m_fd[0];
        fd[1] = pp->m_fd[1];
        handle = (uint64_t)pp.get();
        m_pairs[handle] = std::move(pp);
        return handle;

    tidy_up_and_fail:
        if (connector != -1) {
            closesocket(connector);
        }

        if (acceptor != -1) {
            closesocket(acceptor);
        }

        return 0;
    }

    uint64_t create_pair(SOCKET fd[2]) {
        return create_pair2(AF_INET, SOCK_STREAM, 0, fd);
    }

    void delete_pair(uint64_t handle) {
        if (m_pairs.count(handle) <= 0) {
            return;
        }

        m_pairs.erase(handle);
    }

    void clear() {
        m_pairs.clear();
    }

    struct _pair_type {
        _pair_type() {
            memset(m_fd, -1, sizeof(m_fd));
        }
        ~_pair_type() {
            if (m_fd[0] != -1) {
                closesocket(m_fd[0]);
            }

            if (m_fd[1] != -1) {
                closesocket(m_fd[1]);
            }
        }

        SOCKET m_fd[2];
    };

    typedef std::unique_ptr<_pair_type> _pair_type_ptr;

    SOCKET m_listener;
    sockaddr_in m_listen_addr;
    std::map<uint64_t, _pair_type_ptr> m_pairs;
};

static std::unique_ptr<SocketPair> g_inst;

std::unique_ptr<SocketPair>& SocketPair::instance() {
    if (!g_inst) {
        g_inst.reset(new SocketPair());
    }

    return g_inst;
}

void SocketPair::release() {
    g_inst = nullptr;
}

SocketPair::SocketPair() {
    m_impl = new _impl_t();
}

SocketPair::~SocketPair() {
    delete m_impl;
}

uint64_t SocketPair::create_pair(SOCKET fd[2]) {
    return m_impl->create_pair(fd);
}

void SocketPair::delete_pair(uint64_t handle) {
    m_impl->delete_pair(handle);
}

void SocketPair::clear() {
    m_impl->clear();
}