#pragma once

#include <memory>
#include <mutex>

#define TIMEOUTMS -1 // epoll_wait超时时间，-1表示不设超时
#define CONNECT_TIMEOUT 500 // 连接默认超时时间
#define NUM_WORKERS 4 // 线程池大小

namespace lcx{

class HttpRequest;
class Epoll;
class ThreadPool;

class HttpServer{

public:
    HttpServer(int port, int numThread);
    ~HttpServer();
    void run(); // 服务器启动函数

private:
    void acceptNewConnection(); // 接受新连接
    void closeConnection(); //关闭连接
    void dealRequest();     //处理请求
    void dealResponse();    //处理响应

private:
    using ListenRequestPtr = std::unique_ptr<HttpRequest>;
    using EpollPtr = std::unique_ptr<Epoll>;
    using ThreadPoolPtr = std::shared_ptr<ThreadPool>;

    int mPort;  //监听端口
    int mListenFd; //监听文件描述符

    ListenRequestPtr mListenRequst; //监听套接字的HttpRequest实例
    EpollPtr mEpoll;        //epoll实例
    ThreadPoolPtr mThreadPool;  //线程池
};

} // namespace lcx
