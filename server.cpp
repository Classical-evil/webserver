
#include "src/Server.h"
#include <iostream>
#include "src/Buffer.h"
#include "src/Connection.h"
#include "src/EventLoop.h"
#include "src/Ssocket.h"

int main()
{
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->onConnect([](Connection *conn)
                      {
    conn->Read();
    if (conn->getState() == Connection::State::Closed) {
      conn->close();
      return;
    }
    // std::cout << "Message from client " << conn->getSocket()->getFd() << ": " << conn->ReadBuffer() << std::endl;
    printf("Message from client %d: %s\n", conn->getSocket()->getFd(), conn->ReadBuffer());
    conn->setSendBuffer(conn->ReadBuffer());
    conn->Write(); });

    loop->loop();
    delete server;
    delete loop;
    return 0;
}