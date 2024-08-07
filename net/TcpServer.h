//
// Created by jm on 5/21/24.
//

#ifndef MUDUO_NET_TCPSERVER_H
#define MUDUO_NET_TCPSERVER_H

#include "base/noncopyable.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "Callbacks.h"

#include <string>
#include <netinet/in.h>
#include <map>
#include <functional>
#include <atomic>

class EventLoop;
class EventLoopThreadPool;

class TcpServer : noncopyable {
public:
    enum Option {
        kNoReusePort,
        kReusePort,
    };

    EventLoop* getLoop() const { return loop_; }

    TcpServer(EventLoop* loop, const struct sockaddr_in& listenAddr, std::string &name);
    ~TcpServer();

    void setThreadNum(int numThreads);
    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { ConnectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { MessageCallback_ = cb; }


private:
    // Not thread safe, but in loop
    void newConnection(int sockfd, const struct sockaddr_in& peerAddr);
    void removeConnection(const TcpConnectionPtr&);
    void removeConnectionInLoop(const TcpConnectionPtr&);

private:
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop *loop_;   // the acceptor loop
    std::string name_;
    std::unique_ptr<Acceptor> acceptor_;
    std::shared_ptr<EventLoopThreadPool> threadPool_;

    ConnectionCallback ConnectionCallback_;
    MessageCallback MessageCallback_;

    std::atomic<bool> started_;
    int nextConnId_;

    ConnectionMap connections_;
};


#endif //MUDUO_NET_TCPSERVER_H
