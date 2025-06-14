#include "include/myrpcconfig.h"
#include <iostream>
#include <string>

// 负责解析加载配置文件
void MyrpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r"); // 打开配置文件
    // 如果配置文件是空的
    if (nullptr == pf)
    {
        std::cout << config_file << "is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 循环读取配置文件
    // 1. 注释  2.正确的配置项 =  3.去掉开头多余的空格
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        std::string read_buf(buf);
        Trim(read_buf);

        // 判断#注释
        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            continue; // 说明没找到=
        }

        // 插入键值对
        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        //"      rpcserverip         =     127.0.0.1     \n"(vlaue)
        Trim(key);
        int endidx = read_buf.find('\n', idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);//-1把\n字符去掉
        Trim(value);
        m_configMap.insert({key, value});
    }
}

// 查询key对应的配置项信息
std::string MyrpcConfig::Load(const std::string &key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return " ";
    }
    return it->second;
}

void MyrpcConfig::Trim(std::string &src_buf)
{
    // 去掉字符串前面多余空格
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 如果没有找到空格就返回-1，idx != -1说明找到了,会返回找到的下标
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}