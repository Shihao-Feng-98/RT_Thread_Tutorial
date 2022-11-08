/*
互斥锁
场景：两个线程抢夺同一个资源
作用：每个时刻只有一个线程可以获取锁内的资源，避免资源抢夺
用法：在需要锁的代码块前后加锁和解锁(同一个线程加锁，解锁)
*/

#include <pthread.h> // -lpthread
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// utils
#include <periodic_rt_task.h>

// global variables
int g_num = 10;
pthread_mutex_t g_mutex;//互斥锁


// ======== Thread Function ========  
void* thread_func(void* argc)
{
    // do something
    while(g_num > 0)
    {
        sleep(1);

        pthread_mutex_lock(&g_mutex);

        cout << "[Thread " << pthread_self() << "]: " << g_num-- << endl;
        
        pthread_mutex_unlock(&g_mutex);
    }

    return nullptr;
}


int main(int argc, char **argv)
{
    /*
    mlockall 锁定进程中所有映射到地址空间的页
    MCL_CURRENT 已经映射的进程地址，MCL_FUTURE 将来映射的进程地址
    */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        cout << "mlockall failed: %m\n"; // 输入上一个函数的错误信息
        return -2;
    }
    

    pthread_mutex_init(&g_mutex, NULL); // 动态初始化，记得销毁
    // 创建线程
    PeriodicRtTask *task_1 = new PeriodicRtTask("[Thread 1]", 50, thread_func);
    PeriodicRtTask *task_2 = new PeriodicRtTask("[Thread 2]", 50, thread_func);
    
    sleep(1); 
    
    // 析构函数会join线程，等待子线程结束
    delete task_1;
    delete task_2;

    pthread_mutex_destroy(&g_mutex);

    return 0;
}
