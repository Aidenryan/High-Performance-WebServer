# My High Performance WebServer

- 一个简单的服务器，参考了一些开源代码

- 运行方式

  1. 执行脚本编译

     ```c++
     ./build.sh
     ```

  2. 运行bin文件夹下面的可执行文件

     ```c++
     ./bin/WebServer 8888 4  //8888是端口号  4是线程池中线程数目  这两个可以自己设定
     ```

- 用Webbench测试了一个，可以达到上万的QPS，具体结果可以看看

  [测试结果](https://github.com/Aidenryan/High-Performance-WebServer/blob/main/doc/results.md)

  

