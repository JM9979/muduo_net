//
// Created by jm on 5/26/24.
//

#include "Buffer.h"

#include <sys/uio.h>
#include <cerrno>

// 当数据量大缓冲区不够时, 怎么处理数据呢?
/*
 * 在栈上申请一块65535字节的空间, 使用readv系统调用,
 * 先向缓冲区读入数据, 再读向栈上的空间, 之后将数据append到缓冲区, 缓冲区是自适应的会扩容
 * */
ssize_t Buffer::readFd(int fd, int* saveErrno) {
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const ssize_t n = readv(fd, vec, 2);
    if(n < 0) {
        *saveErrno = errno;
    } else if(n <= writable) {
        writerIndex_ += n;
    } else {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}