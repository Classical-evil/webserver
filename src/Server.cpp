#include "Server.h"
#include "Ssocket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "EventLoop.h"
#include <functional>

Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr)
{
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket *)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    threadPool = new ThreadPool(size);
    for(int i = 0; i < size; i++)
    {
        subReactors.push_back(new EventLoop());
    }

    for(int i = 0; i < size; i++)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        threadPool->add(std::move(sub_loop));
    }


}

Server::~Server()
{
    // delete acceptor;
    // delete threadPool;
}

void Server::newConnection(Socket *sock)
{
    if (sock->getFd() != -1)
    {
        int random = sock->getFd() % subReactors.size();
        Connection *conn = new Connection(subReactors[random], sock);
        std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
        conn->setDeleteConnectCallback(cb);
        conn->setOnConnectCallback(onConnectCallback);
        connections[sock->getFd()] = conn;
    }
}

void Server::deleteConnection(Socket* sock)
{
    int sockfd = sock->getFd();
    if (sockfd != -1)
    {
        
        auto it = connections.find(sockfd);
        if (it != connections.end())
        {
            Connection *conn = connections[sockfd];
            connections.erase(sockfd);
            // close(sockfd);       //正常
            delete conn; // 会Segmant fault
            conn = nullptr;
        }
    }
}

void Server::onConnect(std::function<void(Connection *)> fn) { onConnectCallback = std::move(fn); }