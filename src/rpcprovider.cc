#include "include/rpcprovider.h"
#include<string>
//#include <cstdint>
#include"include/myrpcapplication.h"
#include<functional>

//这是框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    
}

//启动rpc服务节点，开始提供rpc远程网络调用服务
void RpcProvider::Run()
{
    std::string ip = MyrpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MyrpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    //创建Tcpserver对象
    muduo::net::TcpServer server(&m_eventloop, address, "RpcProvider");
    //绑定连接回调和消息读写回调，只需要关心有没有用户连接，以及读写方法（不关心网络）
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));//预留参数位置
    //读写
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    //设置muduo库的线程数量
    server.setThreadNum(4);//自己设置：电脑设置的是4核，这样可以一个线程用于io线程，剩下的是工作线程

    //bin/下面./provider -i test.conf的时候没有输出，这里可以设置一下输出
    std::cout << "RpcProvider start service at ip" << ip << "port" << port << std::endl;

    //启动网络服务
    server.start();
    m_eventloop.loop();
}

//新的socket连接回调
void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn){

}

//已建立连接用户的读写事件回调
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp){

}