#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "Channel.h"

#define MAX_EVENTS 1000

Epoll::Epoll(): epfd(-1), events(nullptr) 
{
    epfd = epoll_create(64);
    errif(epfd == -1, "Epoll create wrong!!");

    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}


Epoll::~Epoll()
{
    if(epfd != -1) 
    {
        close(epfd);
        epfd = -1;
    }

    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = op;

    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev), "Epoll add events error!!");
    
}

std::vector<Channel *> Epoll::poll(int timeout)
{
    std::vector<Channel *> activeChannels;
    // printf("epoll_wait\n");
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i)
    {
        // printf("ptr\n");
        Channel *ch = (Channel *)events[i].data.ptr;
        // printf("setRevents ch:%p\n", ch);
        ch->setRevents(events[i].events);
        // printf("push_back\n");
        activeChannels.push_back(ch);
    }
    // printf("return\n");
    return activeChannels;
}

void Epoll::updateChannel(Channel *channel)
{
    int fd = channel->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (!channel->getInEpoll())
    {
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->setInEpoll();
    }
    else
    {
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}
