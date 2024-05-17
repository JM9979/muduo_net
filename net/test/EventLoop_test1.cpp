//
// Created by jm on 5/17/24.
//

#include "EventLoop.h"
#include <iostream>
#include <unistd.h>
#include <Thread.h>

EventLoop *g_loop;

void threadFunc() {
    g_loop->loop();
}

int main() {
    EventLoop a;
    g_loop = &a;
    Thread t(threadFunc);
    t.start();
    t.join();
}