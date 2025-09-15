#pragma once

#include "Epoller.h"
#include "common.h"
#include <memory>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>
class Epoller;
class EventLoop
{
public:
    DISALLOW_COPY_AND_MOVE(EventLoop);
    EventLoop();
    ~EventLoop();

    void Loop() ;
    void UpdateChannel(Channel *ch) ;
    void DeleteChannel(Channel *ch) ;

    void DoToDoList();

    void QueueOneFunc(std::function<void()> fn);

    void RunOneFunc(std::function<void()> fn);

    bool IsInLoopThread();

    void HandleRead();

private:
    std::unique_ptr<Epoller> poller_;

    std::vector<std::function<void()>> to_do_list_;
    std::mutex mutex_;

    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    bool calling_functors_;
    pid_t tid_;
};