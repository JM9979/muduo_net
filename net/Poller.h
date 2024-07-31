//
// Created by jm on 5/18/24.
//

#ifndef MUDUO_NET_POLLER_H
#define MUDUO_NET_POLLER_H

#include "base/noncopyable.h"
#include "EventLoop.h"

#include <vector>
#include <unordered_map>

class Channel;
struct pollfd;

class Poller : noncopyable {
public:
    typedef std::vector<Channel*> ChannelList;
    Poller(EventLoop* loop);
    virtual ~Poller() = default;

    static Poller* newDefaultPoller(EventLoop* loop);

    void poll(int timeoutMs, ChannelList* activeChannels);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

protected:
    typedef std::unordered_map<int, Channel*> ChannelMap;
    ChannelMap channels_;

private:
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;

    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    EventLoop* ownerLoop_;
};


#endif //MUDUO_NET_POLLER_H
