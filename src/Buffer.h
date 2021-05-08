#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>

#define INIT_SIZE 1024

namespace lcx{

class Buffer
{

public:
    Buffer() : mBuffer(INIT_SIZE), mReaderIndex(0), mWriterIndex(0)
    {
    }
    ~Buffer();

private:

    std::vector<char> mBuffer;
    size_t mReaderIndex;
    size_t mWriterIndex;
};


    
    
} // namespace lcx
