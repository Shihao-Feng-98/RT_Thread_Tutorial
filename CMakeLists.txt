cmake_minimum_required(VERSION 3.12)
project(RT_THREAD)
set(CMAKE_BUILD_TYPE "Debug")
# g++编译选项 
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++11 -Wall")

# 添加头文件路径 -I
include_directories(${CMAKE_SOURCE_DIR}/include)

# 添加源文件路径
# set(SRC) 

# 输出可执行文件 
add_executable(main src/rt_thread.cpp)

# 链接库 -l
target_link_libraries(main PRIVATE 
                        pthread           # C线程库
)

