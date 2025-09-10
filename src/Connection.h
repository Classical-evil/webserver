#pragma once
#include <functional>

class EventLoop;
class Buffer;
class Socket;
class Channel;
class Connection
{


public:

    enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        failed,
    };

    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    void Read();
    void Write();

    void setDeleteConnectCallback(std::function<void(Socket*)> const &callback);
    void setOnConnectCallback(std::function<void(Connection*)> const &callback);
    State getState();
    void close();

    void setSendBuffer(const char *str);
    Buffer *getReadBuffer();
    const char *ReadBuffer();
    Buffer *getSendBuffer();
    const char *SendBuffer();
    void getlineSendBuffer();
    Socket *getSocket();

    void onConnect(std::function<void()> fn);

private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel{nullptr};
    State state{State::Invalid};
    Buffer *readBuffer{nullptr};
    Buffer *sendBuffer{nullptr};
    std::function<void(Socket *)> deleteConnectCallback;

    std::function<void(Connection *)> onConnectCallback;

    void ReadNonBlocking();
    void WriteNonBlocking();
    void ReadBlocking();
    void WriteBlocking();
};