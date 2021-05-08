#pragma once

#include <iostream>
#include <string>
#include <map>
#include "Buffer.h"

#define STATIC_ROOT "../www" //请求资源目录

namespace lcx{

class HttpRequest{

public:
    //枚举类型
    enum HttpRequestParseState{ //Http请求状态
        ExpectRequestLine,
        ExpectHeaders,
        ExpectBody,
        GotAll
    };

    enum Method{ //Http请求方法
        Invalid, Get, Post, Head, Put, Delete
    };

    enum Version{ //Http版本
        Unknown, HTTP10, HTTP11
    };

    HttpRequest(int fd);
    ~HttpRequest();

private:
    //网络通信相关
    int mFd;
    Buffer mReadBuff; //读缓冲区
    Buffer mWriteBuff; // 写缓冲区
    bool mWorking; // 判断是否在工作

    //TODO 定时器相关

    //报文解析相关
    HttpRequestParseState mState;  //报文解析状态
    Method mMethod;     //Http方法
    Version mVersion;   //Http版本
    std::string mURL_Para;   //URL 参数
    std::string mURL_Path;   //URL 路径
    std::map<std::string, std::string> mHeaders;    // 报文头部
};
    
} // namespace lcx
