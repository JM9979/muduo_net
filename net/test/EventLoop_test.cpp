//
// Created by jm on 5/13/24.
//

#include "EventLoop.h"
#include <iostream>
#include <unistd.h>
#include <Thread.h>


void threadFunc() {
    std::cout << "threadFunc: pid = " << getpid() << "  , tid = " << CurrentThread::tid() << std::endl;
    EventLoop loop;
    loop.loop();
}

int main() {
    EventLoop a;
    Thread Thread(threadFunc);
    Thread.start();
    a.loop();
}