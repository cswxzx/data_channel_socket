//
// Created by csw on 2021/7/27.
//

#include "stdafx.h"
#include "thread.h"

struct Thread::_impl_t {
	_impl_t(Thread* p) : m_flag(0), m_running(false), m_p(p), m_runable(nullptr) {}
	typedef std::shared_ptr<std::thread> thread_ptr;
	volatile int m_flag;
	volatile bool m_running;
	Thread* m_p;
	Runable* m_runable;
	thread_ptr m_thread;
	std::mutex m_mutex;

	int start(Runable *runable) {		
		std::unique_lock<std::mutex> lock(m_mutex);

		if (m_thread) {
			return 0;
		}
		
		int ret = m_p->on_start();
		if (ret != 0) {
			return ret;
		}

		m_flag = 1;
		m_runable = runable;
		m_thread = thread_ptr(new std::thread(std::bind(&_impl_t::run, this)));
		return 0;
	}

	int stop() {		
		std::unique_lock<std::mutex> lock(m_mutex);

		if (!m_thread) {
			return 0;
		}
		
		m_flag = 0;
		int ret = m_p->on_stop();
		
		if (m_running && m_thread->joinable()) {
			m_thread->join();
		} else {
			m_thread->detach();
		}

		m_thread.reset();
		return ret;
	}

	bool running() {
		return m_flag > 0;
	}

	int run() {
		int ret = 0;
		m_running = true;
		if (m_runable) {
			ret = m_runable->run();
		} else {
			ret = m_p->run();
		}

		m_running = false;
		return ret;
	}
};

Thread::Thread() : m_impl(nullptr) {
	m_impl = new _impl_t(this);
}

Thread::~Thread() {
	m_impl->stop();
	delete m_impl;
	m_impl = nullptr;
}

int Thread::start(Runable *runable) {
	return m_impl->start(runable);
}

int Thread::stop() {
	return m_impl->stop();
}

int Thread::on_start() {
	return 0;
}

int Thread::on_stop() {
	return 0;
}

bool Thread::running() {
	return m_impl->running();
}