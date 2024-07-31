//
// Created by jm on 7/10/24.
//

#ifndef MUDUO_NET_EVENTLOOPTHREADPOOL_H
#define MUDUO_NET_EVENTLOOPTHREADPOOL_H

#include "base/noncopyable.h"


#include <vector>
#include <memory>
#include <string>
#include <atomic>

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool : noncopyable {
public:
    EventLoopThreadPool(EventLoop* baseloop, std::string& nameArg);
    ~EventLoopThreadPool() = default;
    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start();
    EventLoop* getNextLoop();

private:
    EventLoop* baseloop_;
    std::atomic<bool> started_;
    int numThreads_;
    int next_;
    std::string name_;

    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};


#endif //MUDUO_NET_EVENTLOOPTHREADPOOL_H
