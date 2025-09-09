#include "Channel.h"
#include "Epoll.h"
#include "EventLoop.h"
#include <unistd.h>

Channel::Channel(EventLoop *_ep, int _fd) : ep(_ep), fd(_fd), events(0), ready(0), inEpoll(false)
{
}

Channel::~Channel()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Channel::enableRead()
{
    events = EPOLLIN | EPOLLPRI;
    ep->updateChannel(this);
}

void Channel::useET()
{
    events |= EPOLLET;
    ep->updateChannel(this);
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}
uint32_t Channel::getReady()
{
    return ready;
}

bool Channel::getInEpoll()
{
    return inEpoll;
}

void Channel::setInEpoll(bool _in)
{
    inEpoll = _in;
}

void Channel::setReady(uint32_t _ev)
{
    ready = _ev;
}

void Channel::setReadCallback(std::function<void()> _cb)
{
    readCallback = _cb;
}

void Channel::setUseThreadPool(bool use)
{
    useThreadPool = use;
}

void Channel::handleEvent()
{
    if (ready & (EPOLLIN | EPOLLPRI))
    {
        readCallback();
    }
    if (ready & (EPOLLOUT))
    {
        writeCallback();
    }
}
