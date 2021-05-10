#include "HttpResponse.h"
#include "Buffer.h"

#include <string.h>
#include <iostream>
#include <fcntl.h> // open
#include <unistd.h> // close
#include <sys/stat.h> // stat
#include <sys/mman.h> // mmap, munmap

using namespace lcx;

const std::map<int, std::string> HttpResponse::statusCode2Message = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"}
};

const std::map<std::string, std::string> HttpResponse::suffix2Type = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"}
};

Buffer HttpResponse::makeResponse()
{
    Buffer output;

    if(mStatusCode == 400) 
    {
        doErrorResponse(output, "We can't parse the message");
        return output;
    }
    struct stat fileState;
    // 文件找不到错误
    //获取文件描述符属性，成功返回0 失败-1
    if(::stat(mResourePath.data(), &fileState) < 0)
    {
        mStatusCode = 404;
        doErrorResponse(output, "We can't find the file");
        return output;
    }

    // 权限错误
    if(!(S_ISREG(fileState.st_mode) || !(S_IRUSR & fileState.st_mode))) 
    {
        mStatusCode = 403;
        doErrorResponse(output, "We can't read the file");
        return output;
    }

    // 处理静态文件请求
    doStaticResponse(output, fileState.st_size);
    return output;
}
void HttpResponse::doErrorResponse(Buffer& output, std::string message)
{
    std::string body;
    auto itr = statusCode2Message.find(mStatusCode);
    if(itr == statusCode2Message.end()) 
        return;

    body += "<html><title>Web Response Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    body += std::to_string(mStatusCode) + " : " + itr -> second + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>Lcx Web Server</em></body></html>";

    // 响应行
    output.append("HTTP/1.1 " + std::to_string(mStatusCode) + " " + itr -> second + "\r\n");
    // 报文头
    output.append("Server: Lcx\r\n");
    output.append("Content-type: text/html\r\n");
    output.append("Connection: close\r\n");
    output.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    // 报文体
    output.append(body);
}

void HttpResponse::doStaticResponse(Buffer& output, long fileSize)
{
    assert(fileSize >=0);

    auto itr = statusCode2Message.find(mStatusCode);
    if(itr == statusCode2Message.end()) 
    {
        mStatusCode = 400;
        doErrorResponse(output, "Unknown status code");
        return;
    }

    // 响应行
    output.append("HTTP/1.1 " + std::to_string(mStatusCode) + " " + itr -> second + "\r\n");
    // 报文头
    if(mKeepAlive) 
    {
        output.append("Connection: Keep-Alive\r\n");
        output.append("Keep-Alive: timeout=" + std::to_string(CONNECT_TIMEOUT) + "\r\n");
    } else 
    {
        output.append("Connection: close\r\n");
    }
    output.append("Content-type: " + getFileType() + "\r\n");
    output.append("Content-length: " + std::to_string(fileSize) + "\r\n");
    output.append("Server: Lcx\r\n");
    output.append("\r\n");

    // 报文体
    int srcFd = ::open(mResourePath.data(), O_RDONLY, 0);
    // 存储映射IO
    void* mmapRet = ::mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, srcFd, 0);
    ::close(srcFd);
    if(mmapRet == (void*) -1) 
    {
        munmap(mmapRet, fileSize);
        output.retrieveAll();
        mStatusCode = 404;
        doErrorResponse(output, "We can't find the file");
        return;
    }
    char* srcAddr = static_cast<char*>(mmapRet);
    output.append(srcAddr, fileSize);

    munmap(srcAddr, fileSize);
}

std::string HttpResponse::getFileType()
{
    int idx = mResourePath.find_last_of('.');
    if(idx == std::string::npos) // 找不到文件后缀，默认纯文本
    {
        return "text/plain";
    }

    std::string suffix = mResourePath.substr(idx);
    auto iter = suffix2Type.find(suffix);

    if(iter == suffix2Type.end()) // 未知文件后缀，默认纯文本
    {
        return "text/plain";
    }

    return iter->second;
}