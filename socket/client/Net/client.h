/*************************************************************************
	> File Name: client.h
	> Author: cgn
	> Mail: 
	> Created Time: Mon 05 Nov 2018 11:46:34 AM CST
 ************************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H

#include "def.h"
#include "tools.h"
#include <cstring>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <cerrno>
#include <arpa/inet.h>


class ATClient
{
private:
    int _client_fd;
    int _port;

public: 
    ATClient(unsigned short port = DEFAULT_PORT):
        _port(port)
    {}

    unsigned short port() { return _port; }
    
    /**
     * \bref set port, default port is defined by micro DEFAULT_PORT
     * \param port, port
     */
    void set_port(unsigned short port) { _port = port; }

    bool connect(const std::string& ipaddr = "127.0.0.1");
    void close();
    int send(const void* buf, size_t nbytes);
    int recv(void* buf, size_t nbytes);
};

bool ATClient::connect(const std::string& ipaddr)
{
    if( (this->_client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        LOG("ERROR: create socket error: %s(errno: %d)", std::strerror(errno), errno);
        return false;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_port);

    if( inet_pton(AF_INET, ipaddr.c_str(), &server_addr.sin_addr) == -1)
    {
        LOG("ERROR: ipaddr:%s error: %s(errno: %d)", ipaddr.c_str(), std::strerror(errno), errno);
        return false;
    }

    if( ::connect(_client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        LOG("ERROR: ipaddr:%s error: %s(errno: %d)", ipaddr.c_str(), std::strerror(errno), errno);
        return false;
    }
    
    return true;
}

int ATClient::send(const void* buf, size_t nbytes)
{
    ssize_t ret = ::send(_client_fd, buf, nbytes, 0);
    if((size_t)ret == nbytes)
        return ret;
    else if(ret == -1)
    {
        LOG("ERROR: send error: %s(errno: %d)", std::strerror(errno), errno);
    }
    else
    {
        LOG("ERROR: send return: %ld/%lu, unknown send error: %s(errno: %d)", 
                ret, nbytes, std::strerror(errno), errno);
    }
    return -1;
}

int ATClient::recv(void *buf, size_t nbytes)
{
    ssize_t ret = ::recv(_client_fd, buf, nbytes, 0);
    if(ret > 0)
        return ret;
    else if(ret == -1)
    {
        LOG("ERROR: recv error: %s(errno: %d)", std::strerror(errno), errno);
    }
    else
    {
        LOG("ERROR: recv return: %ld/%lu, unknown recv error: %s(errno: %d)", 
                ret, nbytes, std::strerror(errno), errno);
    }
    return -1;
}

void ATClient::close()
{
    ::close(_client_fd);
}

#endif
