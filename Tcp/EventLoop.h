#pragma once

#include "Epoller.h"
#include "common.h"
#include <memory>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>
#include <thread>

class TimeStamp;
class TimerQueue;
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

    // 定时器功能，
    void RunAt(TimeStamp timestamp, std::function<void()> const &cb);
    void RunAfter(double wait_time, std::function<void()> const &cb);
    void RunEvery(double interval, std::function<void()> const &cb);

private:
    std::unique_ptr<Epoller> poller_;

    std::vector<std::function<void()>> to_do_list_;
    std::mutex mutex_;

    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    bool calling_functors_;
    pid_t tid_;

    std::unique_ptr<TimerQueue> timer_queue_;
};