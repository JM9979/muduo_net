//
// Created by jm on 5/14/24.
// thread_local 存储每线程个独的有数据
//

#ifndef MUDUO_NET_CURRENTTHREAD_H
#define MUDUO_NET_CURRENTTHREAD_H

typedef std::thread::id tid;

namespace CurrentThread {
    // thread_local 线程局部变量
    thread_local tid t_cacheTid;
    thread_local char t_tidString[32];
    thread_local int t_tidStringLength = 6;
    thread_local const char* t_threadName = "default";

    void cacheTid();

    inline tid tid() {
        if(t_cacheTid == std::thread::id()) {
            cacheTid();
        }
        return t_cacheTid;
    }

    inline const char* tidString() { return t_tidString; }

    inline int tidStringLength() { return t_tidStringLength; }

    inline const char* name() { return t_threadName; }
}

#endif //MUDUO_NET_CURRENTTHREAD_H
