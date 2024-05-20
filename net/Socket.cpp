//
// Created by jm on 5/20/24.
//

#include "Socket.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <cstring>
#include <fcntl.h>

Socket::~Socket() {
    ::close(sockfd_);
}

//bool Socket::getTcpinfo(struct tcp_info * tcpi) const {
//    return {};
//}
//
//bool Socket::getTcpInfoString(char *buf, int len) const {
//    return {};
//}


bool setNonblocking(int fd) {
    int old = fcntl(fd, F_GETFL);
    if(old == -1) {
        return false;
    }
    if(fcntl(fd, F_SETFL, old | O_NONBLOCK) == -1) {
        return false;
    }
    return true;
}

int Socket::createNonblocking() {
    int socket_fd = socket(AF_INET, SOL_SOCKET, 0);
    if(socket_fd == -1) {
        return -1;
    }
    if(!setNonblocking(socket_fd)) {
        return -1;
    }
    return socket_fd;
}

void Socket::bindAddress(short port) {
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof server_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if(bind(sockfd_, (struct sockaddr*)&server_addr, sizeof server_addr) == -1) {

    }
}

void Socket::listen() {
    if(::listen(sockfd_, SOMAXCONN) == -1) {

    }
}

int Socket::accept(struct sockaddr_in& client_addr) {
    bzero(&client_addr, sizeof client_addr);
    socklen_t len = sizeof client_addr;
    int connfd = -1;
    if((connfd = ::accept(sockfd_, (struct sockaddr*)& client_addr, &len)) == -1) {
        return -1;
    }
    setNonblocking(connfd);
    return connfd;
}

void Socket::shutdownWrite() {

}

void Socket::setTcpNoDelay(bool on) {
    int op = on ? 1 : 0;
    auto len = static_cast<socklen_t>(sizeof op);
    if(::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &op, len) == -1) {

    }
}

void Socket::setReuseAddr(bool on) {
    int op = on ? 1 : 0;
    auto len = static_cast<socklen_t >(sizeof op);
    if(::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &op, len) == -1) {

    }
}

void Socket::setReusePort(bool on) {
    int op = on ? 1 : 0;
    auto len = static_cast<socklen_t>(sizeof op);
    if(::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &op, len) == -1) {

    }
}

void Socket::setKeepAlive(bool on) {
    int op = on ? 1 : 0;
    auto len = static_cast<socklen_t>(sizeof op);
    if(::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &op, len) == -1) {

    }
}
