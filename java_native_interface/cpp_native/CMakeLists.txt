cmake_minimum_required(VERSION 3.14)
project(hello_jni LANGUAGES CXX)



# 创建动态库目标
add_library(hello SHARED
    src/com_Hello.cpp
    # src/com_Hello.h
)

# 查找 JNI（Java Native Interface）头文件和库
find_package(JNI REQUIRED)
# 添加 JNI 头文件目录
target_include_directories(hello PRIVATE
    ${JNI_INCLUDE_DIRS}
)

