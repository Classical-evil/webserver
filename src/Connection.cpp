#include "Connection.h"
#include "Ssocket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr){
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();
    channel->useET();

    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);
    channel->enableRead();
    channel->setUseThreadPool(true);
    readBuffer = new Buffer();

}

Connection::~Connection(){
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::echo(int sockfd){
    char buf[READ_BUFFER];
    while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if(bytes_read > 0){
            readBuffer->append(buf, bytes_read);
        } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
            printf("continue reading");
            continue;
        } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
            errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
            readBuffer->clear();
            break;
        } else if(bytes_read == 0){  //EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            deleteConnectionCallback(sockfd);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb){
    deleteConnectionCallback = _cb;
}

void Connection::send(int sockfd)
{
    char buf[readBuffer->size() + 3];
    strcpy(buf, readBuffer->c_str());
    ssize_t dataSize = readBuffer->size();
    ssize_t dataLeft = dataSize;
    while(dataLeft > 0)
    {
        ssize_t bytesWrite = write(sockfd, buf + dataSize - dataLeft, dataLeft);

        if (bytesWrite == -1 && errno == EAGAIN)
        {
            break;
        }

        dataLeft -= bytesWrite;
    }
}