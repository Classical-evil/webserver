#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <vector>

EventLoop::EventLoop(): ep(nullptr), quit(false)
{
    ep = new Epoll();
    // threadPool = new ThreadPool();
}

EventLoop::~EventLoop()
{

}

void EventLoop::loop()
{
    while(!quit)
    {
        std::vector<Channel*> chs =  ep->poll();
        for(auto it = chs.begin(); it != chs.end(); it++)
        {
            (*it)->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    ep->updateChannel(channel);
}

// void EventLoop::addThread(std::function<void()> func)
// {
//     threadPool->add(func);
// }