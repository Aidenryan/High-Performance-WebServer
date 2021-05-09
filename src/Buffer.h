#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include <algorithm> //copy search

#define INIT_SIZE 1024  //初始化缓存大小为1024字节

namespace lcx{

class Buffer{
public:
    Buffer() : mBuffer(INIT_SIZE), mReaderIndex(0), mWriterIndex(0)
    {
        assert(readableBytes() == 0);
        assert(writeableBytes() == INIT_SIZE);
    }
    ~Buffer(){}

    size_t readFd(int fd, int* readErrno); // 从套接字读到缓冲区
    size_t writefd(int fd, int* writeError); // 缓冲区写到套接字

    // 可读字节数 ，不能修改成员变量，常成员函数
    inline size_t readableBytes() const
    {
        return mWriterIndex - mReaderIndex;
    }

    //可写字节数
    inline size_t writeableBytes() const
    {
        return mBuffer.size() - mWriterIndex;
    }

    // mReaderIndex前面的缓冲区大小，计算缓冲区中被读取走的数据空间大小
    inline size_t prependableBytes() const
    {
        return mReaderIndex;
    }

    //第一个可读位置
    inline const char* peek() const
    {
        return buffBegin() + mReaderIndex;
    }
    //在初始化mBuffer空间不够用时候，增加空间
    void append(const char* data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data + len, beginWrite());
        mWriterIndex += len; //修改mWriterIndex位置
    }

    void ensureWritableBytes(size_t len) // 确保缓冲区有足够空间
    {
        if(writeableBytes() < len) //确定需要扩充
        {
            makeSpace(len);
        }
        assert(writeableBytes() >= len);
    }

    inline char* beginWrite() { buffBegin() + mWriterIndex; }
    inline const char* beginWrite() const { buffBegin() + mWriterIndex; }

    //找\r\n
    const char* findCRLF() const
    {
        const char CRLF[] = "\r\n";
        const char* crlf = std::search(peek(), beginWrite(), CRLF, CRLF+2);

        return crlf == beginWrite() ? nullptr : crlf;
    }

    void retrieveUntil(const char* end) //更改取出数据标记直到end
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        size_t len = static_cast<size_t>(end - peek());
        assert(len <= readableBytes());
        
        mReaderIndex += len;
    }

private:

    inline char* buffBegin() { return &*mBuffer.begin(); } // 返回缓冲区头指针
    inline const char* buffBegin() const  { return &*mBuffer.begin(); } // 返回缓冲区头指针
    
    void makeSpace(size_t len)  //扩充空间
    {
        if(writeableBytes() + prependableBytes() < len)
        {
            mBuffer.resize(mWriterIndex + len);
        }
        else
        {
            //处理应经被读取走的数据，尽量不增加空间
            size_t readable = readableBytes();
            std::copy(buffBegin() + mReaderIndex, buffBegin() + mWriterIndex, buffBegin());
            mReaderIndex = 0;
            mWriterIndex = mReaderIndex + readable;
            assert(readable == readableBytes());
        }
        
    }

private:

    std::vector<char> mBuffer;
    size_t mReaderIndex;
    size_t mWriterIndex;
};


    
    
} // namespace lcx
