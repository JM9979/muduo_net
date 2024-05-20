//
// Created by jm on 5/20/24.
//

#include "Acceptor.h"
#include "EventLoop.h"

#include <iostream>
#include <unistd.h>

void newConnectionCallback(int connfd, const struct sockaddr_in& client_addr) {
    std::cout << "new connection(): accepted a new connection from " << client_addr.sin_addr.s_addr << std::endl;
    ::close(connfd);
}


int main() {
    std::cout << "main(): pid = " << getpid() << std::endl;

    EventLoop loop;
    Acceptor acceptor(&loop, false, 9000);
    acceptor.setNewConnectionCallback(newConnectionCallback);
    acceptor.listen();

    loop.loop();
}
