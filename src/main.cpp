#include <iostream>
#include "HttpServer.h"
#include <string.h> //basename

int main(int argc, char* argv[])
{
    if( argc <= 2 )
    {
        printf( "usage: %s port_number num_thread\n", basename( argv[0] ) );
        return -1;
    }
    
    //设置端口和线程池线程数目
    int port = atoi( argv[1] );
    int numThread = atoi( argv[2] );

    return 0;
}