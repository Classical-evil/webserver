#pragma once

#include <sys/epoll.h>
#include <functional>

class EventLoop;

class Channel
{
private:
    EventLoop* ep;
    int fd;

    uint32_t events;
    uint32_t revents;
    bool inEpoll;

    std::function<void()> callback;

public:
    Channel(EventLoop* ep, int fd);
    ~Channel();

    void enableReading();
    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();

    bool getInEpoll();
    void setInEpoll();

    void setRevents(uint32_t rev);
    void setCallback(std::function<void()>);
    void handleEvent();
};
