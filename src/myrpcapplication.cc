#include "include/myrpcapplication.h"
#include<iostream>
#include<unistd.h>
#include<string>

void ShowArgsHelp(){
    std::cout<<"format: command -i <configfile>"<<std::endl;
}

//MyrpcApplication 类的 静态方法的类外定义
void MyrpcApplication::Init(int argc,  char **argv)//类外实现静态方法不用带static
{
    //若用户没输入命令行参数（例如没有 -i configfile），则直接报错并退出。
    if(argc < 2){
        ShowArgsHelp();//打印错误//ShowArgsHelp() 是一个函数，用于提示正确的命令格式。
        exit(EXIT_FAILURE);
    }
    //解析命令行参数

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1){//getopt是一个标准函数，用于解析命令行选项（"i:"表示-i必须带一个参数，如配置文件名字）
        switch(c){
            case 'i'://说明有配置文件
                config_file = optarg;
                break;

            case '?':
                //std::cout << "invalid args!" <<std::endl;
                ShowArgsHelp();
                exit(EXIT_FAILURE);
                break;

            case ':':
                //std::cout << "need <configfile>" <<std::endl;
                ShowArgsHelp();
                exit(EXIT_FAILURE);

            default:
                break;
        }
    }
    //开始加载配置文件了（有专门的模块）rpcserver_ip=  rpcserver_port  zookeeper_ip=  zookeeper_port=  
    m_config.LoadConfigFile(config_file.c_str());
}

MyrpcApplication& MyrpcApplication::GetInstance()  //定义唯一的实例
{
    static MyrpcApplication app;
    return app;
} 