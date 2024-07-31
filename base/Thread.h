//
// Created by jm on 5/14/24.
// 封装一个线程, 考虑将单个线程实现好
//

#ifndef MUDUO_NET_THREAD_H
#define MUDUO_NET_THREAD_H

#include "noncopyable.h"
#include "CountDownLatch.h"

#include <functional>
#include <string>
#include <thread>

typedef std::thread::id tid;

class Thread : noncopyable {
public:
    typedef std::function<void ()> ThreadFunc;
    explicit Thread(ThreadFunc  , std::string  name = std::string());
    ~Thread();
    void start();
    void join();

    bool started() const {return started_;}
    tid getTid() const {return tid_;}
    const std::string& name() const {return name_;}
private:
    void setDefaultName();
private:
    bool started_;
    bool joined_;
    tid tid_;
    ThreadFunc func_;
    std::string name_;
    CountDownLatch latch_;
    std::thread thread_;
};


#endif //MUDUO_NET_THREAD_H
