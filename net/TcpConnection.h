//
// Created by jm on 5/21/24.
//

#ifndef MUDUO_NET_TCPCONNECTION_H
#define MUDUO_NET_TCPCONNECTION_H

#include "base/noncopyable.h"
#include "Callbacks.h"
#include "Buffer.h"

#include <memory>
#include <netinet/in.h>
#include <string>

class Socket;
class Channel;
class EventLoop;

class TcpConnection;


class TcpConnection : noncopyable , public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop,
                  std::string  name,
                  int sockfd,
                  const struct sockaddr_in& peerAddr,
                  const struct  sockaddr_in& localAddr);
    ~TcpConnection();

    EventLoop* getLoop() { return loop_; }

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setOnMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }

    void connectEstablished();
    void connectDestroyed();

    void send(const std::string& message);
    void shutdown();

    bool connected() { return state_ == kConnected; }
    const std::string& name() { return name_; }
    const struct sockaddr_in& peerAddr() { return peerAddr_; }
    const struct sockaddr_in& localAddr() { return localAddr_; }

private:
    enum StateE { kConnecting, kConnected, kDisconnecting, kDisconnected };
    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void shutdownInLoop();
    void sendInLoop(const std::string& message);
private:

    EventLoop* loop_;
    const std::string name_;

    StateE state_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const struct sockaddr_in peerAddr_;
    const struct sockaddr_in localAddr_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};


#endif //MUDUO_NET_TCPCONNECTION_H
