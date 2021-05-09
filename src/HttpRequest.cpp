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
            //找\r\n
            const char* crlf = mReadBuff.findCRLF(); 
            if(crlf)
            {
                ok = parseRequestLine(mReadBuff.peek(), crlf);

                if(ok)
                {
                    //取出全部数据后的标记整理
                    mReadBuff.retrieveUntil(crlf + 2);
                    mState = ExpectHeaders;
                }
                else
                {
                    hasMore = false;
                }
            }
            else
            {
                hasMore = false;
            }
        }
        else if(mState == ExpectHeaders)
        {
            //处理报文头
            const char* crlf = mReadBuff.findCRLF();
            if(crlf)
            {
                const char* colon = std::find(mReadBuff.peek(), crlf, ':');
                if(colon != crlf)
                {
                    //处理报文头.以冒号为分界 一行一行处理
                    addHeader(mReadBuff.peek(), colon, crlf);
                }
                else //读完了 报头
                {
                    mState = GotAll;
                    hasMore = false;
                }
                mReadBuff.retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false; 
            }
        }
        else if(mState == ExpectBody)
        {
            // TODO 处理报文体
        }
    }

    return ok;
    
}

std::string HttpRequest::getHeader(const std::string &filed ) const
{
    std::string str;
    auto iter = mHeaders.find(filed);
    if(iter != mHeaders.end())
    {
        str = iter->second;
    }
    return str;
}

bool HttpRequest::keepAlive() const //判断是否为长连接
{
    auto connection = getHeader("Connection");
    bool res = connection == "Keep-Alive" || (mVersion == HTTP11 && connection != "close");
    return res;
}

bool HttpRequest::parseRequestLine(const char* begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');

    if(space != end && setMethod(start, space))//找方法
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if(space != end)//找路径
        {
            const char* question = std::find(start, space, '?');
            if(question != space)
            {

            }
            else
            {
                /* code */
            }

            start = space + 1;
            succeed = (end - start == 8) && std::equal(start, end - 1, "HTTP/1.");
            if(succeed)
            {
                if(*(end - 1) == '1')
                    setHttpVersion(HTTP11);
                else if(*(end - 1) == '0')
                    setHttpVersion(HTTP10);
                else
                    succeed = false;
            }
        }
    }
    return succeed;
}

bool HttpRequest::setMethod(const char* begin, const char* end)
{
    std::string str(begin, end);
    if(str == "GET")
        mMethod = Get;
    else if(str == "POST")
        mMethod = Post;
    else if(str == "HEAD")
        mMethod = Head;
    else if(str == "PUT")
        mMethod = Put;
    else if(str == "DELETE")
        mMethod = Delete;
    else
        mMethod = Invalid;

    return mMethod != Invalid;
}

void HttpRequest::addHeader(const char* start, const char* colon, const char* end)
{

}

void HttpRequest::resetParse()
{
    mState = ExpectRequestLine; // 报文解析状态
    mMethod = Invalid; // HTTP方法
    mVersion = Unknown; // HTTP版本
    mURL_Path = ""; // URL路径
    mURL_Para = ""; // URL参数
    mHeaders.clear(); // 报文头部
}