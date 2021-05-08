#include "Utils.h"
#include <cstring> //bzero
#include <sys/socket.h> //socket setsockopt bind listen
#include <fcntl.h> //fcntl
#include <stdio.h>  //perror
#include <unistd.h> //close
#include <arpa/inet.h> //htonl htons
#include <iostream>
using namespace lcx;

int Utils::createListenFd(int port)
{
    //处理非法端口
    port = ((port <= 1024) || (port >= 65535)) ? 8888 : port;

    //创建套接字 (IPv4，TCP，非阻塞)
    int listenFd;
    if(listenFd = ::socket(AF_INET, SOCK_SEQPACKET | SOCK_NONBLOCK, 0) == -1)
    {
        printf("[Utils::createListenFd]fd = %d socket : %s\n", listenFd, strerror(errno));
        return -1;
    }

    //设置端口复用
    int option = 1;
    if(::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&option, sizeof(int)) == -1)
    {
        printf("[Utils::createListenFd]fd = %d setsockopt : %s\n", listenFd, strerror(errno));
        return -1;
    }

    //设置ip和端口
    sockaddr_in serverAddr; 
    ::bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = ::htons(static_cast<unsigned short>(port));
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);

    //绑定
    if(::bind(listenFd, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("[Utils::createListenFd]fd = %d bind : %s\n", listenFd, strerror(errno));
        return -1;
    }

    //开始监听
    if(::listen(listenFd, LISTENMAX) == -1)
    {
        printf("[Utils::createListenFd]fd = %d listen : %s\n", listenFd, strerror(errno));
        return -1;
    }

    if(listenFd == -1)
    {
        ::close(listenFd);
        return -1;
    }

    return listenFd;
}

int Utils::setNonBlocking(int fd)
{
    //获取文件描述符选项
    int option = ::fcntl(fd, F_GETFL);

    if(option == -1) {
        printf("[Utils::setNonBlocking]fd = %d fcntl : %s\n", fd, strerror(errno));
        return -1;
    }

    //设置非阻塞
    option |= O_NONBLOCK;
    if(::fcntl(fd, F_SETFL, option) == -1) {
        printf("[Utils::setNonBlocking]fd = %d fcntl : %s\n", fd, strerror(errno));
        return -1;
    }
    return 0;

}