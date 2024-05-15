//
// Created by jm on 5/15/24.
// 对系统的一些C++11 条件变量、互斥量一个简单封装, 实现一个信号量的功能
//

#ifndef MUDUO_NET_COUNTDOWNLATCH_H
#define MUDUO_NET_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>


class CountDownLatch {
public:
    explicit CountDownLatch(int count);
    void wait();
    void countDown();
private:
    std::mutex mutex_;
    std::condition_variable condition_;
    int count_;
};


#endif //MUDUO_NET_COUNTDOWNLATCH_H
