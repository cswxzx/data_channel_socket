//
// Created by csw on 2021/7/7.
//

#include "data_channel_socket.h"
#include "socket_pair.h"

struct DataChannelSocket::_impl_t {
	_impl_t(DataChannelSocket *wrapper, rtc::scoped_refptr<webrtc::DataChannelInterface> &channel) : m_wrapper(wrapper), m_channel(channel), m_handle(0) {
		m_buffer.resize(1024);
        memset(m_fd, -1, sizeof(m_fd));
	}

	int start() {
        m_channel->RegisterObserver(m_wrapper);
		m_handle = SocketPair::instance()->create_pair(m_fd);
		if (m_handle == 0) {
            return -1;						
		}

        return 0;
	}

	int stop() {
        m_channel->UnregisterObserver();
		SocketPair::instance()->delete_pair(m_handle);
        return 0;
	}

    int transfer() {
        while (m_wrapper->running()) {
            int ret = read_sink((char*)&m_buffer[0], m_buffer.size());
            if (ret == 0) {
                break;
            }

            if (ret > 0) {
                write_channel((uint8_t*)&m_buffer[0], ret);
            }
        }
        
        return 0;
    }

    int write_source(void *data, size_t size) {
        return ::send(m_fd[0], (char*)data, size, 0);
    }

    int read_source(void *data, size_t size) {
        return ::recv(m_fd[0], (char*)data, size, 0);
    }

    int write_sink(void *data, size_t size) {
        return ::send(m_fd[1], (char*)data, size, 0);
    }

    int read_sink(void *data, size_t size) {
		return ::recv(m_fd[1], (char*)data, size, 0);
    }

    int write_channel(void *data, size_t size) {
        webrtc::DataBuffer buffer(rtc::CopyOnWriteBuffer((uint8_t*)data, size), true);
        return m_channel->Send(buffer) ? size : -1;
    }

    SOCKET get_source_handle() {
        return m_fd[0];
    }

    SOCKET get_sink_handle() {
        return m_fd[1];
    }

	DataChannelSocket *m_wrapper;
    rtc::scoped_refptr<webrtc::DataChannelInterface> m_channel;
	std::string m_buffer;
	SOCKET m_fd[2];
	uint64_t m_handle;    
};

DataChannelSocket::DataChannelSocket(rtc::scoped_refptr<webrtc::DataChannelInterface> &channel) {
	m_impl = new _impl_t(this, channel);
}

DataChannelSocket::~DataChannelSocket() {
	delete m_impl;
}

void DataChannelSocket::OnStateChange() {

}

void DataChannelSocket::OnMessage(const webrtc::DataBuffer &buffer) {
    if (!running()) {
        return;
    }

    if (buffer.size() <= 0) {        
        return;
    }

    uint8_t *data = (uint8_t*)buffer.data.data();
    size_t size = buffer.size();
    m_impl->write_sink(data, size);
}

int DataChannelSocket::on_start() {
    m_impl->start();
    return DataChannel::on_start();
}

int DataChannelSocket::on_stop() {
    m_impl->stop(); 
    return DataChannel::on_stop();
}

int DataChannelSocket::run() {
	return m_impl->transfer();
}

int DataChannelSocket::send(void *data, size_t size) {
    webrtc::DataBuffer buffer(rtc::CopyOnWriteBuffer((uint8_t*)data, size), true);
    return Send(buffer) ? size : -1;
}

int DataChannelSocket::recv(void *data, size_t size) {
    return m_impl->read_source(data, size);
}

SOCKET DataChannelSocket::get_source_handle() {
    return m_impl->get_source_handle();
}