
set(net_SRCS
        EventLoop.cpp
        Channel.cpp
        Poller.cpp
        EventLoopThread.cpp
        Socket.cpp
        Acceptor.cpp
        TcpServer.cpp
        TcpConnection.cpp
)

add_library(muduo_net ${net_SRCS})
target_link_libraries(muduo_net muduo_base)

install(TARGETS muduo_net DESTINATION lib)

#set(HEADERS
#        EventLoop.h
#)
#install(FILES ${HEADERS} DESTINATION include/muduo/net)


add_subdirectory(test)

