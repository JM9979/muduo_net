//
// Created by jm on 5/14/24.
//

#include "Thread.h"
#include "CurrentThread.h"

#include <cassert>
#include <sys/prctl.h>


namespace CurrentThread {
    // thread_local 线程局部变量
    thread_local ::tid t_cacheTid;
    thread_local char t_tidString[32];
    thread_local int t_tidStringLength = 6;
    thread_local const char* t_threadName = "default";
}

tid getTid() { return std::this_thread::get_id(); }

void CurrentThread::cacheTid() {
    t_cacheTid = getTid();
}

struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    tid* tid_;
    CountDownLatch* latch_;

    ThreadData(ThreadFunc func, std::string name, tid* tid, CountDownLatch* latch)
        : func_(std::move(func)), name_(std::move(name)), tid_(tid), latch_(latch) { }

    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = nullptr;
        latch_->countDown();
        latch_ = nullptr;
        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);

        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void* startThread(void* obj) {
    auto* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return nullptr;
}

Thread::Thread(ThreadFunc  func, std::string n)
    : started_(false),
    joined_(false),
    tid_(),
    func_(std::move(func)),
    name_(std::move(n)),
    latch_(1)
    {
        setDefaultName();
    }

Thread::~Thread() {
    if(started_ && !joined_) thread_.detach();
}

void Thread::setDefaultName() {
    if(name_.empty()) {
        name_ = "Thread";
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    auto* data = new ThreadData(func_, name_, &tid_, &latch_);
    thread_ = std::move(std::thread(startThread, data));
    latch_.wait();
}

void Thread::join() {
    assert(started_);
    assert(!joined_);
    if(thread_.joinable()) {
        joined_ = true;
        thread_.join();
    }
}