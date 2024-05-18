//
// Created by jm on 5/18/24.
//


#include "EventLoop.h"
#include "Channel.h"

#include <sys/timerfd.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

EventLoop* g_loop;

void timeout() {
    std::cout << "Timeout !" << std::endl;
    g_loop->quit();
}

int main() {
    EventLoop loop;
    g_loop = &loop;

    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);

    loop.loop();
    ::close(timerfd);
}

