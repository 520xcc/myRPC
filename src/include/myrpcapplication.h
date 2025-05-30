#pragma once //防止头文件被重复包含

#include"myrpcconfig.h"

//rpc框架的基础类。初始化类。负责初始化工作
class MyrpcApplication
{
public:
    static void Init(int argc,  char **argv);
    static MyrpcApplication& GetInstance();  //定义唯一的实例
    static MyrpcConfig& GetConfig();//提供给别人获取配置
    // {
    //     static MyrpcApplication app;
    //     return app;
    // } 
private:
    static MyrpcConfig m_config;//初始化的时候需要调用配置文件
//单例模式设计
    MyrpcApplication(){}
    MyrpcApplication(const MyrpcApplication&) = delete;
    MyrpcApplication(MyrpcApplication&&) = delete;
};