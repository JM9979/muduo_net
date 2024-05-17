//
// Created by jm on 5/13/24.
//

#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "noncopyable.h"
#include "CurrentThread.h"

#include <functional>
#include <cassert>
#include <atomic>

class EventLoop : noncopyable {
public:
    typedef std::function<void ()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInloop(Functor&& cb);
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    void assertInLoopThread() const { assert(isInLoopThread()); }
private:
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    const tid threadId_;
};

#endif //MUDUO_NET_EVENTLOOP_H
