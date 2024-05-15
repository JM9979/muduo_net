//
// Created by jm on 5/15/24.
//

#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) :count_(count) {}

void CountDownLatch::wait() {
    std::unique_lock<std::mutex> lock_(mutex_);
    condition_.wait(lock_, [this](){return count_ == 0;});
}

void CountDownLatch::countDown() {
    std::unique_lock<std::mutex> lock_(mutex_);
    --count_;
    if(count_ == 0) condition_.notify_all();
}
