//
// Created by jm on 5/20/24.
//
// todo: 没有处理文件描述符用光的情况


#include "Acceptor.h"
#include "EventLoop.h"

#include <unistd.h>

Acceptor::Acceptor(EventLoop *loop, bool reuseport, unsigned short port)
    : loop_(loop),
      acceptSocket_(Socket::createNonblocking()),
      acceptChannel_(loop, acceptSocket_.fd()),
      listenning_(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reuseport);
    acceptSocket_.bindAddress(port);

    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
}

void Acceptor::listen() {
    loop_->assertInLoopThread();
    listenning_ = true;

    acceptSocket_.listen();

    acceptChannel_.enableReading();
}

// 没有处理文件描述符用光的情况
void Acceptor::handleRead() {
    loop_->assertInLoopThread();
    struct sockaddr_in client_addr;
    int connfd = acceptSocket_.accept(client_addr);
    if(connfd >= 0) {
        if(NewConnectionCallback_) {
            NewConnectionCallback_(connfd, client_addr);
        }
    } else {
        ::close(connfd);
    }
}