//
// Created by jm on 5/18/24.
//

#ifndef MUDUO_NET_CHANNEL_H
#define MUDUO_NET_CHANNEL_H

#include "noncopyable.h"

#include <functional>
#include <memory>
#include <atomic>

class EventLoop;

class Channel : noncopyable {
public:
    typedef std::function<void ()> EventCallback;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void tie(const std::shared_ptr<void>&);

    void handleEvent();
    void setReadCallback(const EventCallback& cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback& cb) { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback& cb) { errorCallback_ = cb; }
    void setCloseCallback(const EventCallback& cb) { closeCallback_ = cb; }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    bool isReading() const { return events_ & kReadEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }


    int index() const { return index_; }
    void set_index(int idx) { index_ = idx; }

    EventLoop* ownerLoop() { return loop_; }

private:
    void update();
    void remove();

private:

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop* loop_;
    const int fd_;

    std::atomic<bool> eventHanding_;

    int events_;
    // poll返回的事件
    int revents_;
    // 记录下标的
    int index_;

    bool tied_;
    std::weak_ptr<void> tie_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};


#endif //MUDUO_NET_CHANNEL_H
