第一步：写test.proto
文件目录下运行：protoc test.proto --cpp_out=./

第二步：写main.cc
文件目录下运行：g++ *.cc -lprotobuf

![alt text](image.png)

代码框架\
![alt text](image-1.png)\

1.      example是简单业务实现
|___callee是服务提供者\
|___caller是服务的调用者


2.      src是RPC框架的源文件

