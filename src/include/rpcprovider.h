#pragma once
#include "google/protobuf/service.h"
#include<memory>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
//框架提供的专门服务发布的rpc服务的网络对象类
class RpcProvider{
public:
    //这是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    //启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();
private:
    //因为只有run方法调用了，因此这里写进 run 方法里面
    //组合了TcpServer
    //std::unique_ptr<muduo::net::TcpServer> m_tcpserverptr;
    //组合Eventloop
    muduo::net::EventLoop m_eventloop;
    //新的socket连接回调
    void onConnection(const TcpConnectionPtr&);
    //一建立连接用户的读写事件回调
    void (const TcpConnectionPtr&, Buffer*, Timestamp)
};