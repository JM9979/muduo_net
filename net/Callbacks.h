//
// Created by jm on 5/21/24.
//

#ifndef MUDUO_NET_CALLBACKS_H
#define MUDUO_NET_CALLBACKS_H

#include <memory>
#include <functional>

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;


typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const TcpConnectionPtr&, const char* , size_t)> MessageCallback;

#endif //MUDUO_NET_CALLBACKS_H
