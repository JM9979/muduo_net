//
// Created by jm on 5/18/24.
//

#include "Poller.h"

#include <poll.h>

Poller::Poller(EventLoop* loop) :ownerLoop_(loop) {}

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    return new Poller(loop);
}


void Poller::poll(int timeoutMs, Poller::ChannelList *activeChannels) {
    int numEvents = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);
    if(numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
    } else if(numEvents < 0) {

    }
}

void Poller::fillActiveChannels(int numEvents, Poller::ChannelList *activeChannels) const {
    for(auto &pfd : pollfds_) {
        if(pfd.revents > 0) {
            --numEvents;
            auto ch = channels_.find(pfd.fd);
            assert(ch != channels_.end());
            Channel* channel = ch->second;
            assert(channel->fd() == pfd.fd);
            channel->set_revents(pfd.revents);
            activeChannels->emplace_back(channel);
        }
    }
}

void Poller::updateChannel(Channel *channel) {
    assertInLoopThread();
    if(channel->index() < 0) {
        // 新频道
        assert(channels_.find(channel->fd()) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        pollfds_.emplace_back(pfd);
        int index = static_cast<int>(pollfds_.size()) - 1;
        channel->set_index(index);
        channels_[pfd.fd] = channel;
    } else {
        // 更新频道
        assert(channels_.find(channel->fd()) != channels_.end());
        assert(channels_[channel->fd()] == channel);
        int index = channel->index();
        assert(0 <= index && index < static_cast<int>(pollfds_.size()));
        struct pollfd &pfd = pollfds_[index];
        assert(pfd.fd == channel->fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if(channel->isNoneEvent()) {
            pfd.fd = -1;
        }
    }
}

void Poller::removeChannel(Channel *channel) {

}
