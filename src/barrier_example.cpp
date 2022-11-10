/*
屏障
场景：多个线程，执行到某一步的时候需要等待所有线程同时运行
*/

#include <pthread.h> // -lpthread
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
#include <C_timer.h>
#include <periodic_rt_task.h>

// global variables
pthread_barrier_t g_barr;
bool g_stop_all = false;

// ======== Main Thread Function ========  
void* main_thread_func(void* argc)
{
    CTimer timer_step, timer_total;
    double time_since_run = 0.;
    const double dt = 0.002;
    unsigned int iteration_main = 0;
    
    pthread_barrier_wait(&g_barr);

    timer_total.reset();
    while (time_since_run < 1.)
    {
        timer_step.reset();

        time_since_run += dt;
        ++iteration_main;
        while(timer_step.end() < dt*1000*1000);
    }
    double end = timer_total.end()/1000/1000;
    usleep(dt/2*1000*1000);
    g_stop_all = true;
    
    cout << "Actual time: " << end << " s\n";
    cout << "Iteration main: " << iteration_main << endl;

    return nullptr;
}

// ======== Child Thread Function ========  
void* child_thread_1_func(void* argc)
{
    CTimer timer_step;
    const double dt = 0.002;
    unsigned int iteration_child = 0;

    pthread_barrier_wait(&g_barr);

    while(1)
    {
        timer_step.reset();
        if (g_stop_all) {break;}

        usleep(1000);

        ++iteration_child;
        while(timer_step.end() < dt*1000*1000);
    }
    cout << "Iteration child 1: " << iteration_child << endl;

    return nullptr;
}

// ======== Child Thread Function ========  
void* child_thread_2_func(void* argc)
{
    CTimer timer_step;
    const double dt = 0.002;
    unsigned int iteration_child = 0;

    pthread_barrier_wait(&g_barr);

    while(1)
    {
        timer_step.reset();
        if (g_stop_all) {break;}

        usleep(1000);

        ++iteration_child;
        while(timer_step.end() < dt*1000*1000);
    }
    cout << "Iteration child 2: " << iteration_child << endl;

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
    
    // 初始化屏障
    pthread_barrier_init(&g_barr, NULL, 3);

    // 创建线程
    PeriodicRtTask *child_task_1 = new PeriodicRtTask("[Child Thread 1]", 95, child_thread_1_func);
    PeriodicRtTask *child_task_2 = new PeriodicRtTask("[Child Thread 2]", 95, child_thread_2_func);
    PeriodicRtTask *main_task = new PeriodicRtTask("[Main Thread]", 95, main_thread_func);

    // 合并线程
    delete child_task_1;
    delete child_task_2;
    delete main_task;

    // 销毁屏障
    pthread_barrier_destroy(&g_barr);

    return 0;
}
