# 当前目录下的所有源文件
aux_source_directory(. SRC_LIST)
# set(SRC_LIST myrpcapplication.cc myrpcconfig.cc rpcheader.pb.cc rpcprovider.cc)
#编译成library
add_library(myrpc ${SRC_LIST}) #去掉SHARED 静态库

#依赖Muduo库
target_link_libraries(myrpc muduo_net muduo_base pthread)