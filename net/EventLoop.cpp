//
// Created by jm on 5/13/24.
//

#include "EventLoop.h"
#include "Poller.h"

#include <poll.h>

thread_local EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop()
    : looping_(false),
    threadId_(CurrentThread::tid()),
    quit_(false),
    poller_(Poller::newDefaultPoller(this))
    {
    if(t_loopInThisThread != nullptr) {
        // 致命错误, 这个线程已经有一个事件循环
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    quit_ = false;
    looping_ = true;
    while(!quit_) {
        activeChannels_.clear();
        poller_->poll(5, &activeChannels_);
        for(auto &iter : activeChannels_) {
            iter->handleEvent();
        }
    }
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::runInLoop(Functor&& cb) {

}

void EventLoop::queueInloop(Functor&& cb) {

}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}
