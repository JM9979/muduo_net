//
// Created by jm on 7/10/24.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, std::string& nameArg)
    : baseloop_(baseloop),
    started_(false),
    numThreads_(0),
    next_(0),
    name_(std::move(nameArg))
    { }

void EventLoopThreadPool::start() {
    assert(!started_);
    baseloop_->assertInLoopThread();
    started_ = true;
    printf("%d\n", numThreads_);
    for(int i = 0; i < numThreads_; ++i) {
        char buf[name_.size() + 32];
        snprintf(buf, sizeof buf, "%s->%d", name_.c_str(), i);
        auto t = new EventLoopThread(buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    baseloop_->assertInLoopThread();
    assert(started_);
    EventLoop* loop = baseloop_;
    // 线程分配算法, round-robin
    if(!loops_.empty()) {
        loop = loops_[next_];
        ++next_;
        if(next_ >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

