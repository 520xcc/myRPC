#include "include/rpcprovider.h"
#include"include/myrpcapplication.h"
#include"rpcheader.pb.h"
/*
service_name => service描述
                           =>service* 记录服务对象
                           =>method_name => method方法对象
        servicemap{service_name->serviceDesc<service*, method_name->method>}
*/
//这是框架提供给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    //创建服务描述对象
    ServiceInfo service_info;

    //获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    //获取服务名字
    std::string service_name = pserviceDesc->name();
    //获取服务对象service的方法的数量
    int methodCnt = pserviceDesc->method_count();

    //打印信息
    std::cout<<"service_name:"<< service_name <<std::endl;

    for(int i = 0; i < methodCnt; ++i){
        //获取了服务对象指定下标的服务方法的描述（抽象描述）
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);

        //获取服务方法名
        std::string method_name = pmethodDesc->name();
        //填表
        service_info.m_methodMap.insert({method_name, pmethodDesc});

        //打印信息
        std::cout<<"method_name:"<< method_name<<std::endl;
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
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
    if(!conn->connected()){
        //和rpc client断开连接
        conn->shutdown();//对应socket的close
    }
}


/*
请求者发调用请求需要携带参数，怎么组织：在框架内部，发布者与消费者要协商之间通信的protobuf数据类型
service_name method_name args
框架定义了proto的message类型，进行数据头的序列化与反序列化
16UserServiceLoginzhang san123456粘包问题（定义好args_size
header_size + header_str + args_str
*/
//已建立连接用户的读写事件回调，完成数据的序列化与反序列化，如果远程有一个远程的RPC服务调用请求，onMessage会响应
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn, 
                            muduo::net::Buffer *buffer, 
                            muduo::Timestamp)
{
    //网络上接收的远程rpc调用请求--->字节流（字符流）,包含方法名：Login
    std::string recv_buf = buffer->retrieveAllAsString();

    //从字符流中读取前4个字节的内容
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);//将读取的头长度拷贝进来copy(_CharT* __s, size_type __n, size_type __pos)

    //根据header_size读取数据头的原始字节流
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    myrpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    //反序列化数据得到原始的详细信息
    if(rpcHeader.ParseFromString(rpc_header_str)){
        //数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
        std::cout << "service_name: " << service_name << std::endl;
        std::cout << "method_name: " << method_name << std::endl;
        std::cout << "args_size: " << args_size << std::endl;
    }else{
        //数据头反序列化失败
        std::cout << " rpc_header_str: " << rpc_header_str << " parse error! " << std::endl;
        return;
    }

    //获取rpc方法参数的字符流数据
    std::string args_str = recv_buf.substr(4 + header_size);

    //打印调试信息
    std::cout << "=============================================" << std::endl;
    std::cout << "header_size:" << header_size << std::endl;
    std::cout << "rpc_header_str:" << rpc_header_str << std::endl;
    std::cout << "service_name:" << service_name << std::endl;
    std::cout << "method_name:" << method_name << std::endl;
    std::cout << "args_str:" << args_str << std::endl;
    std::cout << "=============================================" << std::endl;

    //获取service对象和method对象
    //先判断有没有这个服务
    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end()){//没有这个服务
        std::cout << service_name <<"is not exist !"<< std::endl;
        return;
    }
    //再判断有没有这个方法
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end()){//没有这个方法
        std::cout << service_name << ":" << method_name <<"is not exist !"<< std::endl;
        return;
    }
    //取service对象
    google::protobuf::Service *service = it->second.m_service;
    //取method对象 
    const google::protobuf::MethodDescriptor *method = mit->second;
    

    //反序列化结束后获取了服务对象和方法，现在需要调用服务，调用的是发布者提供的逻辑
    //生成rpc方法调用的请求request和响应的response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str)){
        std::cout << "request parse error! content:" << args_str <<std::endl;
        return;
    }//解析请求
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //给下面method方法的调用，绑定一个Closure的回调函数
    //inline Closure* NewCallback
    //(Class* object, void (Class::*method)(Arg1),Arg1 arg1)
    google::protobuf::Closure *done = 
                            google::protobuf::NewCallback<RpcProvider, 
                                                            const muduo::net::TcpConnectionPtr&, 
                                                            google::protobuf::Message*>
                                                            (this, &RpcProvider::sendRpcResponse, conn, response);
    //在框架上，根据远端的RPC请求，调用当前rpc节点发布的方法
                        // (::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                        // const ::fixbug::LoginRequest*,
                        // ::fixbug::LoginResponse*,
                        // ::google::protobuf::Closure* done)
    service->CallMethod(method, nullptr, request, response, done);//相当于userservice调用login()方法
}

//Closure回调操作，用于序列化RPC的响应和网络发送
void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string response_str;
    if(response->SerializeToString(&response_str))//response进行序列化
    {
        //序列化成功后，通过网络将rpc执行结果响应发送给rpc调用方
        conn->send(response_str);
    }else{
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown();//模拟http短链接。有rpcprovider主动断开连接
}