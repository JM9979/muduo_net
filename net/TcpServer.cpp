//
// Created by jm on 5/21/24.
//

#include "TcpServer.h"
#include "EventLoop.h"

#include <cstring>

TcpServer::TcpServer(EventLoop *loop, const struct sockaddr_in &listenAddr, std::string &name)
        : loop_(loop),
          name_(name),

          acceptor_(new Acceptor(loop, kNoReusePort, listenAddr.sin_port)),
          started_(false),
          nextConnId_(1)
        {
            acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
        }

TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
}


void TcpServer::start() {
    if(!started_) {
        started_ = true;
    }
    if(!acceptor_->listenning()) {
//        loop_->runInLoop(std::bind(&Acceptor::listen, this));
    }
}

void TcpServer::newConnection(int sockfd, const struct sockaddr_in &peerAddr) {
    loop_->assertInLoopThread();

    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    std::string connName = name_ + buf;

    struct sockaddr_in localAddr;
    socklen_t len = sizeof localAddr;
    bzero(&localAddr, sizeof localAddr);

    if(::getsockname(sockfd, (struct sockaddr*)&localAddr, &len) == - 1) {
        // LOG_SYS
    }

    TcpConnectionPtr conn =
            std::make_shared<TcpConnection>(loop_, connName, sockfd, localAddr, peerAddr);

    connections_[connName] = conn;

    conn->setConnectionCallback(ConnectionCallback_);
    conn->setOnMessageCallback(MessageCallback_);
    conn->connectEstablished();
}


