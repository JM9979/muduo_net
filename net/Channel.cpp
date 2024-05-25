//
// Created by jm on 5/18/24.
//

#include "Channel.h"
#include "EventLoop.h"

#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    tied_(false),
    eventHanding_(false)
{
}

Channel::~Channel() {
    assert(!eventHanding_);
}

void Channel::handleEvent() {
    eventHanding_ = true;
    if(revents_ & POLLNVAL) {

    }
    if((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if(closeCallback_) closeCallback_();
    }
    if(revents_ & (POLLERR | POLLNVAL)) {
        if(errorCallback_) errorCallback_();
    }
    if(revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if(readCallback_) readCallback_();
    }
    if(revents_ & POLLOUT) {
        if(writeCallback_) writeCallback_();
    }
    eventHanding_ = false;
}

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::remove() {
    assert(isNoneEvent());
    loop_->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<void> & obj) {
    tie_ = obj;
    tied_ = true;
}


