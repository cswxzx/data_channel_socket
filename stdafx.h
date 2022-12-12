//
// Created by csw on 2021/6/30.
//

#ifndef PROJECT_STDAFX_H
#define PROJECT_STDAFX_H


#if defined(WEBRTC_WIN)
#ifndef _UNISTD_H
#define _UNISTD_H 
#include <io.h> 
#include <process.h>
#endif 
#include <cstdint>
#include <atlbase.h>
#include <atlstr.h>
#else
#include <unistd.h>
#endif


#include <thread>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <mutex>
#include <functional>
#include <chrono>
#include <string>
#include <condition_variable>
#include <array>

#endif //PROJECT_STDAFX_H
