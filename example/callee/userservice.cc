#include <iostream>
#include <string>
#include "../user.pb.h"
#include "myrpcapplication.h"
#include "rpcprovider.h"
//using namespace fixbug;
/*
UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
*/

class UserService : public fixbug :: UserServiceRpc//注意要添加名字作用域，否则这里会出现问题
{ 
public:
    bool Login(std::string name, std::string pwd)//用户的登录服务，这是本地业务
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name" << name << "pwd" << pwd << std::endl;
    }
    //如何将这个本地方法，变为rpc远程方法——>框架要做的事
    //第一步：（方法名、参数、返回值）序列化

    /*重写基类userServiceRpc的虚函数，下面的方法都是RPC框架直接调用
    1、caller想要发起远程的RPC请求===>调用Login(LoginRequest)===>muduo===>callee
    2、callee知道远端要请求Login(LoginRequest)==>交给下面重写的Login(LoginRequest)方法上
    */
    void Login(::google::protobuf::RpcController* controller,
        const ::fixbug::LoginRequest* request,
        ::fixbug::LoginResponse* response,
        ::google::protobuf::Closure* done)
    {
        //框架给业务上报请求的服务Login(LoginRequest)，应用程序根据本地服务取出相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        //本地业务
        bool login_result = Login(name, pwd);

        //将响应写入，包括错误码、错误消息、返回值。返回给RPC框架
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        //执行回调操作
        done->Run();
    }
};

int main(int argc, char **argv){
    //调用框架的初始化操作   ----希望用户传入命令行参数：【provider -i config】
    MyrpcApplication::Init(argc, argv);

    //提供服务的发布者与消费者，provider是一个rpc网络服务对象，把服务发布到rpc节点上
    RpcProvider provider; //负责数据序列化反序列化以及收发
    provider.NotifyService(new UserService());
    //provider.NotifyService(new xxxService());

    //启动一个rpc服务节点，Run以后，进程会进入阻塞状态，等待远程rpc调用请求
    provider.Run();

    return 0;
}