//
// Created by jm on 5/19/24.
//

#ifndef MUDUO_NET_EVENTLOOPTHREAD_H
#define MUDUO_NET_EVENTLOOPTHREAD_H

#include "Thread.h"
#include "noncopyable.h"

#include <atomic>
#include <mutex>
#include <condition_variable>


class EventLoop;

class EventLoopThread : noncopyable {
public:
    explicit EventLoopThread(const std::string& name = std::string());
    ~EventLoopThread();
    EventLoop* startLoop();
    const std::string& name() const { return thread_.name(); }

private:
    void threadFunc();

private:
    EventLoop* loop_;
    std::mutex mutex_;
    std::condition_variable cond_;

    std::atomic<bool> exiting_;
    Thread thread_;
};


#endif //MUDUO_NET_EVENTLOOPTHREAD_H
