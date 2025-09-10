#pragma once 

#include <map>
#include <vector>
#include <functional>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop* mainReactor;
    std::vector<EventLoop*> subReactors;
    Acceptor *acceptor;
    std::map<int, Connection *> connections;
    ThreadPool* threadPool;
    std::function<void (Connection*)> onConnectCallback;

public:
    Server(EventLoop*);
    ~Server();

    // void handleEvent(int);
    void newConnection(Socket* serv_sock);
    void deleteConnection(Socket* sock);
    void onConnect(std::function<void (Connection*)> func);
};