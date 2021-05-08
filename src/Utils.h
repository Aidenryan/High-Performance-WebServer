#pragma once

#define LISTENMAX 1024 // 监听队列长度,操作系统默认值为SOMAXCONN

namespace lcx
{
    namespace Utils
    {
        int createListenFd(int port);
        int setNonBlocking(int fd);
    } // namespace Utils
    
} // namespace lcx
