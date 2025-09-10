#include "Connection.h"
#include "Ssocket.h"
#include "Channel.h"
#include "Buffer.h"
#include "util.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr){
    if (loop != nullptr)
    {
        channel = new Channel(loop, sock->getFd());
        channel->enableRead();
        channel->useET();
    }

    readBuffer = new Buffer();
    sendBuffer = new Buffer();
    state = State::Connected;
}

Connection::~Connection(){
    if (channel != nullptr)
        delete channel;
    delete sock;
    delete readBuffer;
    delete sendBuffer;
}

void Connection::Read()
{
    assert(state == State::Connected);
    readBuffer->clear();
    ReadNonBlocking();;
}

void Connection::Write()
{
    assert(state == State::Connected);
    WriteNonBlocking();
    sendBuffer->clear();
}

void Connection::ReadNonBlocking()
{
    int sockfd = sock->getFd();
    char buf[1024]; // 这个buf大小无所谓
    while (true)
    { // 使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        memset(buf, 0, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            readBuffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        { // 程序正常中断、继续读取
            printf("continue reading\n");
            continue;
        }
        else if (bytes_read == -1 &&
                 ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { // 非阻塞IO，这个条件表示数据全部读取完毕
            break;
        }
        else if (bytes_read == 0)
        { // EOF，客户端断开连接
            printf("read EOF, client fd %d disconnected\n", sockfd);
            state = State::Closed;
            break;
        }
        else
        {
            printf("Other error on client fd %d\n", sockfd);
            state = State::Closed;
            break;
        }
    }
}

void Connection::WriteNonBlocking()
{
    int sockfd = sock->getFd();
    char buf[sendBuffer->size()];
    memcpy(buf, sendBuffer->c_str(), sendBuffer->size());
    int data_size = sendBuffer->size();
    int data_left = data_size;
    while (data_left > 0)
    {
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if (bytes_write == -1 && errno == EINTR)
        {
            printf("continue writing\n");
            continue;
        }
        if (bytes_write == -1 && errno == EAGAIN)
        {
            break;
        }
        if (bytes_write == -1)
        {
            printf("Other error on client fd %d\n", sockfd);
            state = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}

void Connection::close() {deleteConnectCallback(sock);}

Connection::State Connection::getState() {return state;}
void Connection::setSendBuffer(const char *str) { sendBuffer->setBuf(str); }
Buffer *Connection::getReadBuffer() { return readBuffer; }
const char *Connection::ReadBuffer() { return readBuffer->c_str(); }
Buffer *Connection::getSendBuffer() { return sendBuffer; }
const char *Connection::SendBuffer() { return sendBuffer->c_str(); }

void Connection::setDeleteConnectCallback(std::function<void(Socket *)> const &callback) {
  deleteConnectCallback = callback;
}

void Connection::setOnConnectCallback(std::function<void(Connection *)> const &callback) {
  onConnectCallback = callback;
  channel->setReadCallback([this]() { onConnectCallback(this); });
}

void Connection::getlineSendBuffer() { sendBuffer->getline(); }

Socket *Connection::getSocket() { return sock; }