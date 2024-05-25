//
// Created by jm on 5/13/24.
// EventLoop 保证每个线程都只有一个事件循环 “one loop per thread”的基础
//

#ifndef MUDUO_NET_EVENTLOOP_H
#define MUDUO_NET_EVENTLOOP_H

#include "noncopyable.h"
#include "CurrentThread.h"
#include "Channel.h"

#include <functional>
#include <cassert>
#include <atomic>
#include <vector>
#include <memory>
#include <mutex>

class Poller;

class EventLoop : noncopyable {
public:
    typedef std::function<void ()> Functor;
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();
    void runInLoop(const Functor& cb);
    void queueInloop(const Functor& cb);
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    void assertInLoopThread() const { assert(isInLoopThread()); }

    void wakeup() const;
    void updateChannel(Channel*);
    void removeChannel(Channel*);
private:

    void doPendingFunctors();
    void handleRead() const; // wakeup 读取数据出处理函数


private:
    typedef std::vector<Channel*> ChannelList;
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    std::atomic<bool> callingPendingFunctors_;
    const tid threadId_;
    ChannelList activeChannels_;

    // 实现跨线程间的通信, 统一事件处理
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    std::unique_ptr<Poller> poller_;
    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_;
};

#endif //MUDUO_NET_EVENTLOOP_H
