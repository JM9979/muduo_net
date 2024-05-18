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

class Poller;

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

    void updateChannel(Channel*);
private:
    typedef std::vector<Channel*> ChannelList;
    std::atomic<bool> looping_;
    std::atomic<bool> quit_;
    const tid threadId_;
    ChannelList activeChannels_;

    std::unique_ptr<Poller> poller_;
};

#endif //MUDUO_NET_EVENTLOOP_H
