//
// Created by csw on 2021/7/27.
//

#ifndef GEELEVEL_THREAD_H
#define GEELEVEL_THREAD_H

#include "stdafx.h"

struct Runable {
	virtual int run() = 0;
};

class Thread {
public:
	Thread();
	virtual ~Thread();
public:
	int start(Runable *runable = nullptr);
	int stop();
	bool running();
protected:
	virtual int on_start();
	virtual int on_stop();
	virtual int run() { return 0; };
private:
	struct _impl_t;
	_impl_t* m_impl;
};


#endif //GEELEVEL_THREAD_H
