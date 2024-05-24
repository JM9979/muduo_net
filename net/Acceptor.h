//
// Created by jm on 5/20/24.
//

#ifndef MUDUO_NET_ACCEPTOR_H
#define MUDUO_NET_ACCEPTOR_H

#include "noncopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>
#include <netinet/in.h>

class Acceptor : noncopyable {
public:
    typedef std::function<void (int sockfd, const struct sockaddr_in&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, bool reuseport, unsigned short port);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { NewConnectionCallback_ = cb; }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

private:
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    bool listenning_;
//    int idleFd_;

    NewConnectionCallback NewConnectionCallback_;
};


#endif //MUDUO_NET_ACCEPTOR_H
