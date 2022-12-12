//
// Created by csw on 2021/7/7.
//

#ifndef PROJECT_DATA_CHANNEL_SOCKET_H
#define PROJECT_DATA_CHANNEL_SOCKET_H

#include "stdafx.h"
#include "thread.h"
#include "socket_define.h"
#include "api/scoped_refptr.h"
#include "api/data_channel_interface.h"

class DataChannelSocket : public Thread, public webrtc::DataChannelObserver {
public:
    DataChannelSocket(rtc::scoped_refptr<webrtc::DataChannelInterface> &channel);
    virtual ~DataChannelSocket();
private:
    void OnStateChange();
    void OnMessage(const webrtc::DataBuffer &buffer);
public:
    int send(void *data, size_t size);
    int recv(void *data, size_t size);
    SOCKET get_source_handle();
protected:
    int on_start();
    int on_stop();
    int run();
private:
    struct _impl_t;
    _impl_t *m_impl;
};


#endif //PROJECT_DATA_CHANNEL_SOCKET_H
