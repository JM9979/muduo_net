//
// Created by jm on 5/21/24.
//

#include "TcpConnection.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"


#include <unistd.h>


TcpConnection::TcpConnection(EventLoop *loop,
                             std::string name,
                             int sockfd,
                             const struct sockaddr_in &peerAddr,
                             const struct sockaddr_in &localAddr)
                             : loop_(loop),
                             name_(std::move(name)),
                             socket_(new Socket(sockfd)),
                             channel_(new Channel(loop, sockfd)),
                             peerAddr_(peerAddr),
                             localAddr_(localAddr),
                             state_(kConnecting) {

    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));

    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    assert(state_ == kConnected);
}

void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead() {
    char buf[65536];
    size_t n = ::read(channel_->fd(), buf, sizeof buf);
    messageCallback_(shared_from_this(), buf, n);
}