//
// Created by jm on 5/15/24.
//

#include "Thread.h"
#include <iostream>

void func() {
    std::cout << "hello, Tid is " << std::this_thread::get_id() << std::endl;
}

int main() {

    Thread myThread(std::bind(func));
    Thread myThread1(std::bind(func));

    myThread.start();
    myThread1.start();

    myThread.join();
    myThread1.join();
}
