//
// Created by jm on 5/21/24.
//

#include "TcpServer.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

#include <cstring>
#include <memory>

TcpServer::TcpServer(EventLoop *loop, const struct sockaddr_in &listenAddr, std::string &name)
        : loop_(loop),
          name_(name),

          acceptor_(new Acceptor(loop, kNoReusePort, listenAddr.sin_port)),
          started_(false),
          nextConnId_(1),
          threadPool_(std::make_shared<EventLoopThreadPool>(loop, name) )
        {
            acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
        }

TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
}


void TcpServer::setThreadNum(int numThreads) {
    assert(0 <= numThreads);
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    if(!started_) {
        started_ = true;
    }
    threadPool_->start();
    if(!acceptor_->listenning()) {
        loop_->runInLoop([this]() { this->acceptor_->listen(); });
    }
}

void TcpServer::newConnection(int sockfd, const struct sockaddr_in &peerAddr) {
    loop_->assertInLoopThread();

    EventLoop* ioLoop = threadPool_->getNextLoop();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    struct sockaddr_in localAddr{};
    socklen_t len = sizeof localAddr;
    bzero(&localAddr, sizeof localAddr);

    if(::getsockname(sockfd, (struct sockaddr*)&localAddr, &len) == - 1) {
        // LOG_SYS
    }

    TcpConnectionPtr conn =
            std::make_shared<TcpConnection>(ioLoop, connName, sockfd, peerAddr, localAddr);

    connections_[connName] = conn;

    conn->setConnectionCallback(ConnectionCallback_);
    conn->setOnMessageCallback(MessageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));

    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    size_t n = connections_.erase(conn->name());
    assert(n == 1);
    EventLoop* ioLoop = conn->getLoop();

    ioLoop->queueInloop(std::bind(&TcpConnection::connectDestroyed, conn));
}


