#pragma once
#include <memory>
#include <list>
#include <iostream>

class ITcpClientHandler
{
public:
    void OnRead(const char* buf, size_t len)
    {
        std::cout << "Client OnRead: " << std::string(buf, len) << std::endl;
    }
};

class TcpClientPrivate;
class TcpClient
{
public:
    TcpClient(const char* ip, int port);
    ~TcpClient();

    void AddHandler(std::shared_ptr<ITcpClientHandler> handler);
    void Run();
    void Close();
    void Write(const char* buf, int len);

private:
    std::unique_ptr<TcpClientPrivate> priv;
};
