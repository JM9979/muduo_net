//
// Created by jm on 5/13/24.
//

#include "EventLoop.h"
#include "Poller.h"

#include <poll.h>
#include <sys/eventfd.h>

thread_local EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventFd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evtfd < 0) {
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false),
    threadId_(CurrentThread::tid()),
    quit_(false),
    wakeupFd_(createEventFd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    callingPendingFunctors_(false),
    poller_(Poller::newDefaultPoller(this))
    {
    if(t_loopInThisThread != nullptr) {
        // 致命错误, 这个线程已经有一个事件循环
    } else {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
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
        poller_->poll(kPollTimeMs, &activeChannels_);
        for(auto &iter : activeChannels_) {
            iter->handleEvent();
        }
        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb) {
    if(isInLoopThread()) {
        cb();
    } else {
        queueInloop(cb);
    }
}

void EventLoop::queueInloop(const Functor& cb) {
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }

    if(!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(auto &iter : functors) {
        iter();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::handleRead() const {
    eventfd_t one;
    int ret = eventfd_read(wakeupFd_, &one);
    if(ret == -1) {

    }
}

void EventLoop::wakeup() const {
    int n = eventfd_write(wakeupFd_, 1);
    if(n == -1 ) {

    }
}

void EventLoop::updateChannel(Channel* channel) {
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}
