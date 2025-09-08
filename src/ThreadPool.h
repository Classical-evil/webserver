#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable cv;
    bool stop;

public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    void add(std::function<void()> func);

};