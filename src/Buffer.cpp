#include "Buffer.h"
#include <sys/uio.h>
#include <string.h>
#include <unistd.h>
using namespace lcx;

size_t Buffer::readFd(int fd, int* readErrno)
{
    //保证一次性读到足够多数据
    char extraBuff[65536];
    iovec vec[2];
    const size_t wirteable = writeableBytes(); //获取可写字节数
    vec[0].iov_base = buffBegin() + mWriterIndex; //读取区域起始位置
    vec[0].iov_len = wirteable;
    vec[1].iov_base = extraBuff;
    vec[1].iov_len = sizeof(extraBuff);

    //readv则将从fd读入的数据按同样的顺序散布到各缓冲区中，readv总是先填满一个缓冲区，然后再填下一个
    const ssize_t readn = ::readv(fd, vec, 2);
    if(readn < 0)
    {
        printf("[Buffer:readFd]fd = %d readv : %s\n", fd, strerror(errno));
        *readErrno = errno;
    }
    else if(static_cast<size_t>(readn) <= wirteable)
    {
        mWriterIndex += readn;//移动mBuffer的已用标记
    }
    else
    {
        mWriterIndex = mBuffer.size();
        append(extraBuff, readn - wirteable); //扩容
    }

    return readn;
    
}

size_t Buffer::writefd(int fd, int* writeError)
{
    size_t nLeft = readableBytes();
    char* buffPtr = buffBegin() + mReaderIndex;

    size_t writen;
    if((writen = ::write(fd, buffPtr, nLeft)) <= 0)
    {
        //writen == EINTR => errno == EINTR
        if(writen < 0 && errno == EINTR)
        {
            return 0;
        }
        else
        {
            printf("[Buffer:writeFd]fd = %d write : %s\n", fd, strerror(errno));
            *writeError = errno;
            return -1;
        } 
    }
    else
    {
        mReaderIndex += writen;
        return writen;
    }
    
}