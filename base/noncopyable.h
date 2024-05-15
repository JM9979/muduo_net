//
// Created by jm on 5/13/24.
// 继承这个类的继承者是不可拷贝的
//

#ifndef MUDUO_NET_NONCOPYABLE_H
#define MUDUO_NET_NONCOPYABLE_H

class noncopyable {
public:
    noncopyable(const noncopyable& other) = delete;
    noncopyable& operator=(const noncopyable& other) = delete;
public:
    noncopyable() = default;
    ~noncopyable() = default;

};

#endif //MUDUO_NET_NONCOPYABLE_H
