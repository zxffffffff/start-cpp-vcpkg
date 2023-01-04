#pragma once
#include <memory>
#include <list>
#include <iostream>

class ITcpServerHandler
{
public:
    void OnRead(const char* buf, size_t len) 
    {
        std::cout << "Server OnRead: " << std::string(buf, len) << std::endl;
    }
};

class TcpServerPrivate;
class TcpServer
{
public:
    TcpServer(const char* ip, int port);
    ~TcpServer();

    void AddHandler(std::shared_ptr<ITcpServerHandler> handler);
    void Run();
    void Close();

private:
    std::unique_ptr<TcpServerPrivate> priv;
};
