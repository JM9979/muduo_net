//
// Created by jm on 5/19/24.
//

#include "EventLoopThread.h"
#include "EventLoop.h"

#include <functional>
#include <string>

EventLoopThread::EventLoopThread(const std::string& name)
    : loop_(nullptr),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::threadFunc, this), name) { }


EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if(loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    assert(!thread_.started());
    thread_.start();

    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        cond_.wait(lock_, [this]() { return loop_ != nullptr; });
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    {
        std::unique_lock<std::mutex> lock_(mutex_);
        loop_ = &loop;
        cond_.notify_all();
    }
    loop.loop();

    std::unique_lock<std::mutex> lock_(mutex_);
    loop_ = nullptr;
}
