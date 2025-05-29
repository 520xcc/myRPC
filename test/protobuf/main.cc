#include <iostream>
#include "test.pb.h"
using namespace fixbug;

// int main1()
// {
//     //封装了1ogin请求对象的数据
//     LoginRequest req;
//     req.set_name("zhang san");
//     req.set_pwd("123456");

//     std::string send_str;//创建要发送数据对象
//     //对象数据序列化 =》 char*字符流形式如果序列化成功
//     if(req.SerializeToString(&send_str)){
//         std::cout << send_str.c_str() << std::endl;
//     }
//     LoginRequest reqB;//创建接收数据对象
//     //从send_str反序列化一个1egin请求对象
//     if(reqB.ParseFromString(send_str)){
//         std::cout << reqB.name() << std::endl;
//         std::cout << reqB.pwd() << std::endl;
//     }
//     return 0;
// }

int main(){
    GetFriendListsResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_error(0);
    //rc->set_errmsg("有错误的话输入错误的消息，没有错误不用管");   没有错误就不用设置errmsg

    User *user1 = rsp.add_friend_list();
    user1->set_name("zhangsan");
    user1->set_age(20);
    user1->set_sex(User::MAN);

    User *user2 = rsp.add_friend_list();
    user2->set_name("lisa");
    user2->set_age(19);
    user2->set_sex(User::WOMAN);

    std::cout << rsp.friend_list_size() << std::endl;
    std::cout << rsp.friend_list(0).name() << std::endl;
    std::cout << rsp.friend_list(1).name() << std::endl;
    
    
    return 0;
}




