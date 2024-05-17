//
// Created by jm on 5/13/24.
//

#include "EventLoop.h"

thread_local EventLoop* t_loopInThisThread = nullptr;

EventLoop::EventLoop() : looping_(false), threadId_(CurrentThread::tid()), quit_(false) {
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
