#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Thread.h"

#include <iostream>
#include <unistd.h>

void print(EventLoop* p = nullptr)
{
    std::cout << "pid = " << getpid() << ", tid = " << CurrentThread::tid() << ", loop = " << p << std::endl;
}

void quit(EventLoop* p)
{
    print(p);
    p->quit();
}

int main()
{
    print();

    {
        EventLoopThread thr1;  // never start
    }

    {
        // dtor calls quit()
        EventLoopThread thr2;
        EventLoop* loop = thr2.startLoop();
        loop->runInLoop(std::bind(print, loop));
//        CurrentThread::sleepUsec(500 * 1000);
    }

    {
        // quit() before dtor
        EventLoopThread thr3;
        EventLoop* loop = thr3.startLoop();
        loop->runInLoop(std::bind(quit, loop));
//        CurrentThread::sleepUsec(500 * 1000);
    }
}

