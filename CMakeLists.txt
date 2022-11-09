cmake_minimum_required(VERSION 3.12)
project(RT_THREAD)
set(CMAKE_BUILD_TYPE "Debug")
# g++编译选项 
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -std=c++11 -Wall")

# 添加头文件路径 -I
include_directories(${CMAKE_SOURCE_DIR}/include)

# # 输出可执行文件 
# add_executable(mutex_example src/mutex_example.cpp)
# # 链接库 -l
# target_link_libraries(mutex_example PRIVATE pthread)

# # 输出可执行文件 
# add_executable(sem_example src/sem_example.cpp)
# # 链接库 -l
# target_link_libraries(sem_example PRIVATE pthread)

# # 输出可执行文件 
# add_executable(cond_example src/cond_example.cpp)
# # 链接库 -l
# target_link_libraries(cond_example PRIVATE pthread)

# # 输出可执行文件 
# add_executable(barrier_example src/barrier_example.cpp)
# # 链接库 -l
# target_link_libraries(barrier_example PRIVATE pthread)

# # 输出可执行文件 
# add_executable(rt_example src/rt_thread_example.cpp)
# # 链接库 -l
# target_link_libraries(rt_example PRIVATE pthread)

# 输出可执行文件 
add_executable(multi_thread_example src/multi_thread_example.cpp)
# 链接库 -l
target_link_libraries(multi_thread_example PRIVATE pthread)
