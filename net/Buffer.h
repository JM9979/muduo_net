//
// Created by jm on 5/26/24.
//

#ifndef MUDUO_NET_BUFFER_H
#define MUDUO_NET_BUFFER_H

#include <vector>
#include <cstring>
#include <cassert>
#include <aio.h>
#include <algorithm>
#include <string>

class Buffer {
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t InitialSize = kInitialSize)
        :buffer_(kCheapPrepend + InitialSize),
        readIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == InitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    size_t readableBytes() const { return writerIndex_ - readIndex_; }
    size_t writableBytes() const { return buffer_.size() - writerIndex_; }
    size_t prependableBytes() const { return readIndex_; }

    const char* peek() const { return begin() + readIndex_; }

    void hasWritten(size_t len) {
        assert(len <= writableBytes());
        writerIndex_ += len;
    }

    void retrieve(size_t len) {
        assert(len <= readableBytes());
        if(len < readableBytes()) {
            readIndex_ += len;
        } else {
            retrieveAll();
        }
    }

    void retrieveInt64() {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32() {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16() {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8() {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll() {
        readIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    // 以string形式返回数据
    std::string retrieveAllAsString() {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString(size_t len) {
        assert(len <= readableBytes());
        std::string  result(peek(), len);
        retrieve(len);
        return result;
    }


    void append(const char* data, size_t len) {
        ensureWritableBytes(len);

        std::copy(data, data + len, begin() + writerIndex_);

        hasWritten(len);
    }

    void append(const void* data, size_t len) {
        append(static_cast<const char *>(data), len);
    }

    ssize_t readFd(int fd, int* saveErrno);

private:
    char* begin() { return &*buffer_.begin(); }
    const char* begin() const { return &*buffer_.begin(); }

    void ensureWritableBytes(size_t len) {
        if(writableBytes() < len) {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    void makeSpace(size_t len) {
        if(writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(writerIndex_ + len);
        } else {
            assert(kCheapPrepend < readIndex_);
            size_t readable = readableBytes();

            std::copy(begin() + readIndex_, begin() + writerIndex_, begin() + kCheapPrepend);

            readIndex_ = kCheapPrepend;
            writerIndex_ = readIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writerIndex_;
};


#endif //MUDUO_NET_BUFFER_H
