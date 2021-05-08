#pragma once

#include <string>
#include <map>

namespace lcx
{

class HttpResponse
{
public:
    HttpResponse(int statusCode, std::string path, bool keepAlive):
    mStatusCode(statusCode), mResourePath(path), mKeepAlive(keepAlive){}
    ~HttpResponse();

private:
    std::map<std::string, std::string> mHeaders; // 头部响应报文
    int mStatusCode; //响应状态码
    std::string mResourePath; //请求资源路径
    bool mKeepAlive; //是否是长连接
};

    
} // namespace lcx
