#pragma once

#include <string>
#include <map>

#define CONNECT_TIMEOUT 500 // 非活跃连接500ms断开

namespace lcx{

class Buffer;

class HttpResponse
{
public:

    static const std::map<int, std::string> statusCode2Message; //响应状态码
    static const std::map<std::string, std::string> suffix2Type;  //文件类型对应

    HttpResponse(int statusCode, std::string path, bool keepAlive):
    mStatusCode(statusCode), mResourePath(path), mKeepAlive(keepAlive){}
    ~HttpResponse(){}

    Buffer makeResponse();
    void doErrorResponse(Buffer& output, std::string message);
    void doStaticResponse(Buffer& output, long fileSize);

private:

    std::string getFileType();

    std::map<std::string, std::string> mHeaders; // 头部响应报文
    int mStatusCode; //响应状态码
    std::string mResourePath; //请求资源路径
    bool mKeepAlive; //是否是长连接
};

    
} // namespace lcx
