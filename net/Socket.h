//
// Created by jm on 5/20/24.
//

#ifndef MUDUO_NET_SOCKET_H
#define MUDUO_NET_SOCKET_H

#include "noncopyable.h"

class Socket : noncopyable {
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) { }

    ~Socket();

    int fd() const { return sockfd_; }

//    bool getTcpinfo(struct tcp_info*) const;
//    bool getTcpInfoString(char* buf, int len) const;

    static int createNonblocking();

    void bindAddress(short port);

    void listen();

    int accept(struct sockaddr_in&);

    void shutdownWrite();

    void setTcpNoDelay(bool on);

    void setReuseAddr(bool on);

    void setReusePort(bool on);

    void setKeepAlive(bool on);

private:
    const int sockfd_;
};


#endif //MUDUO_NET_SOCKET_H
