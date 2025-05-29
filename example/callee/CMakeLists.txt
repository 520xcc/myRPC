set(SRC_LIST userservice.cc ../user.pb.cc)
add_executable(provider ${SRC_LIST})

#要用到框架代码，动态链接
target_link_libraries(provider myrpc protobuf)