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
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));

    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    assert(state_ == kDisconnected);
}

void TcpConnection::connectEstablished() {
    loop_->assertInLoopThread();
    assert(state_ == kConnecting);
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    setState(kDisconnected);
    channel_->disableAll();
    // ?
    connectionCallback_(shared_from_this());

    loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead() {
    char buf[65536];
    ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
    if (n > 0) {
        messageCallback_(shared_from_this(), buf, n);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleWrite() {

}

void TcpConnection::handleError() {

}
