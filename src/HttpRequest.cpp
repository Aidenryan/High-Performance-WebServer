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

}

int HttpRequest::reqWrite(int *WriteError)
{
    
}