#pragma once
#include "google/protobuf/service.h"
#include<unordered_map>
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>

#include "google/protobuf/descriptor.h"
#include<string>
#include<functional>

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

    /*
    
    service_name => service描述（struct ServiceInfo）
                           =>service* 记录服务对象
                           =>method_name => method方法对象
    */
    //service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service; //保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; //保存服务方法
    };
    //存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    //新的socket连接回调
    void onConnection(const muduo::net::TcpConnectionPtr&);

    //已建立连接用户的读写事件回调
    void onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);

    //Closure回调操作，用于序列化RPC的响应和网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);

};