#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Ssocket.h"
#include "Channel.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void handleReadEvent(int sockfd);

int main()
{
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    
    Epoll *ep = new Epoll();
    serv_sock->setnonblocking();
    // ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);

    Channel *servChannel = new Channel(ep, serv_sock->getFd());
    servChannel->enableReading();
    printf("while\n");
    while(true)
    {
        printf("epoll\n");
        std::vector<Channel*> channels = ep->poll();
        int nfds = channels.size();
        printf("for\n");
        for(int i = 0; i < nfds; i++)
        {
            if(channels[i]->getFd() == serv_sock->getFd()) 
            {
                InetAddress* clnt_addr = new InetAddress();
                Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr));

                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking();
                Channel* clnt_channel = new Channel(ep, clnt_sock->getFd());
                clnt_channel->enableReading();
            }
            else if (channels[i]->getRevents() & EPOLLIN)
            { // 可读事件
                handleReadEvent(channels[i]->getFd());
            }
            else
            { // 其他事件，之后的版本实现
                printf("something else happened\n");
            }
        }
       
        
    }

    delete serv_sock;
    delete serv_addr;

    return 0;
}

void handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    { // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        }
        else if (bytes_read == -1 && errno == EINTR)
        { // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if (bytes_read == 0)
        { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd); // 关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}