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

    inline int fd(){ return mFd; }//返回文件描述符
    int reqRead(int *readError);    //读数据
    int reqWrite(int *writeError);  //写数据

    inline int writeableBytes() { return mWriteBuff.readableBytes(); }

    inline void setWorking() {mWorking = true;}  //设置工作状态
    inline void setNoWorking() {mWorking = false;}
    inline bool isWorking() const {return mWorking;} //判断是否在工作

    bool parseRequest();//解析Http报文
    inline bool finishParse() {return mState == GotAll;}    // 是否解析完一个报文
    void resetParse();  // 重置解析状态

    std::string getHeader(const std::string &filed ) const;
    bool keepAlive() const; //判断是否为长连接

private:
    //解析请求行
    bool parseRequestLine(const char* begin, const char* end);
    //增加报文头
    void addHeader(const char* start, const char* colon, const char* end);
    //设置请求方法
    bool setMethod(const char* begin, const char* end);
    //设置URL路径
    void setURLPath(const char* begin, const char* end)
    {
        std::string subPath;
        subPath.assign(begin, end);
        if(subPath == "/")
            subPath = "/index.html";
        mURL_Path = STATIC_ROOT + subPath; //设置资源请求路径
    }
    //设置URL参数
    void setURLPara(const char* begin, const char* end){ mURL_Para.assign(begin, end); }
    //设置Http版本
    void setHttpVersion(Version version) {mVersion = version; };

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
