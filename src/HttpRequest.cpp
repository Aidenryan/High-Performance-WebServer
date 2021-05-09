#include "HttpRequest.h"
#include <unistd.h>
using namespace lcx;

HttpRequest::HttpRequest(int fd) : mFd(fd), mWorking(false), //TODO
                                    mState(ExpectRequestLine), mMethod(Invalid), mVersion(Unknown)
{
    assert(mFd >= 0);
}

HttpRequest::~HttpRequest()
{
    close(mFd);
}

int HttpRequest::reqRead(int *readError)
{
    int ret = mReadBuff.readFd(mFd, readError);
    return ret;
}

int HttpRequest::reqWrite(int *writeError)
{
    int ret = mWriteBuff.writefd(mFd, writeError);
    return ret;
}

bool HttpRequest::parseRequest()
{
    bool ok = true;
    bool hasMore = true;

    while (hasMore)
    {
        if(mState == ExpectRequestLine)
        {
            const char* crlf = mReadBuff.findCRLF();
        }
    }
    
}

bool HttpRequest::keepAlive() const //判断是否为长连接
{

}

void HttpRequest::resetParse()
{

}