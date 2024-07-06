//
// Created by jm on 7/6/24.
//

//
// Created by jm on 5/24/24.
//

#include "TcpServer.h"
#include "EventLoop.h"
#include "Buffer.h"

#include <unistd.h>
#include <cstring>

class Buffer;

void onConnection(const TcpConnectionPtr& conn) {
    if(conn->connected()) {
        printf("onConnection(): connection [%s] from %d : %d\n",
               conn->name().c_str(), conn->peerAddr().sin_addr.s_addr, conn->peerAddr().sin_port);

        conn->send("Hello\r\n");
        conn->send("I am going to write more data\r\n");
        conn->shutdown();
    } else {
        printf("onConnection(): connection [%s] is down\n", conn->name().c_str());
    }
}

void onMessage(const TcpConnectionPtr & conn, Buffer* data, size_t n) {
    printf("onMessage: received %zd bytes from connection [%s]\n", n, conn->name().c_str());
    printf("onMessage: %s\n", data->retrieveAsString(n).c_str());
}

int main() {
    printf("main(): pid = %d\n", getpid());

    struct  sockaddr_in listenAddr;
    bzero(&listenAddr, sizeof listenAddr);
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    listenAddr.sin_port = 9000;

    EventLoop loop;
    std::string name = "TcpServer";
    TcpServer server(&loop, listenAddr, name);

    server.setConnectionCallback(onConnection);
    server.setMessageCallback(onMessage);

    server.start();
    loop.loop();
}