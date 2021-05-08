#pragma once

namespace lcx
{
    namespace Utils
    {
        int createListenFd(int port);
        int setNonBlocking(int fd);
    } // namespace Utils
    
} // namespace lcx
