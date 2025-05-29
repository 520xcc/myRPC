#pragma once

#include<unordered_map>
#include<string>

//创建这个类是用于读取配置文件
//主要涉及rpcserverip/rpcserverport/zookeeperip/zookeeperport四个配置文件
//在这个类中创建了两个方法：
//【1】负责解析加载配置文件
//【2】负责根据key值返回对应的配置文件

//框架读取配置文件类
class MyrpcConfig{
public:
    //负责解析加载配置文件
    void LoadConfigFile(const char *config_file);
    //查询key对应的配置项信息
    std::string Load(const std::string &key);
private:
    //存储键值对类型
    std::unordered_map<std::string, std::string> m_configMap;
};