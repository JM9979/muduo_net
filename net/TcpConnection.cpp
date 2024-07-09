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
    if(state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    loop_->removeChannel(channel_.get());
}

void TcpConnection::shutdown() {
    if(state_ == kConnected) {
        setState(kDisconnecting);
        loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::send(const std::string &message) {
    if(state_ == kConnected) {
        if(loop_->isInLoopThread()) {
            sendInLoop(message);
        } else {
            loop_->runInLoop(std::bind(&TcpConnection::sendInLoop, this, message));
        }
    }
}

void TcpConnection::handleRead() {
    int saveErrno = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveErrno);
    if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuffer_, n);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleClose() {
    loop_->assertInLoopThread();
    assert(state_ == kConnected || state_ == kDisconnecting);
    setState(kDisconnected);
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleWrite() {
    loop_->assertInLoopThread();
    if(channel_->isWriting()) {
        ssize_t n = ::write(channel_->fd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if(n > 0) {
            outputBuffer_.retrieve(n);
            if(outputBuffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if(state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            } else {
                printf("I am going to write more data");
            }
        } else {
            printf("TcpConnection::handleWrite");
        }
    } else {
        printf("Connection is down, no more writing");
    }
}

void TcpConnection::handleError() {

}


void TcpConnection::shutdownInLoop() {
    loop_->assertInLoopThread();
    if(!channel_->isWriting()) {
        socket_->shutdownWrite();
    }
}
void TcpConnection::sendInLoop(const std::string& message) {
    loop_->assertInLoopThread();
    ssize_t nwrite = 0;
    if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrite = ::write(channel_->fd(), message.data(), message.size());
        if(nwrite >= 0) {
            if ((size_t) nwrite < message.size()) {
                printf("I am going to write more data");
            }
        } else {
            nwrite = 0;
            if(errno != EWOULDBLOCK) {

            }
        }
    }

    assert(nwrite >= 0);
    if((size_t)nwrite < message.size()) {
        outputBuffer_.append(message.data() + nwrite, message.size() - nwrite);
        if(!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}