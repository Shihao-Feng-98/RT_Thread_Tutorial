/*
Example for single real-time thread with CPU binding
*/

#include <pthread.h> // -lpthread
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// utils
#include <C_timer.h>
#include <periodic_rt_task.h>

// gobal variable
double dt_controller = 0.001;
double time_since_run = 0.; 
int iteration = 0;

// ======== Main Control Thread Function ========  
void* main_control_loop(void* argc)
{
    cpu_set_t mask; // CPU核心的集合
    CPU_ZERO(&mask); // 清空集合
    CPU_SET(0, &mask); // 将CPU核心0加入到集合中
    if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) != 0){
        cout << "Cound not set CPU affinity\n";
    }
    // else{ // just for check
    //     cpu_set_t affinity; 
    //     CPU_ZERO(&affinity); 
    //     pthread_getaffinity_np(pthread_self(), sizeof(affinity), &affinity);
    //     for (int i = 0; i < 16; i++)
    //     {
    //         if (CPU_ISSET(i, &affinity)) // 判断和哪个CPU绑定
    //         {
    //             cout << "Thread is running in processor " << i << endl;
    //         }
    //     }
    // }

    CTimer timer_step, timer_total;
    cout << "[Main Control Thread]: thread start\n";

    // run periodic task
    while(time_since_run <= 0.02)
    {
        // timer reset
        timer_step.reset();
        time_since_run += dt_controller;
        iteration++;

        // do something
        cout << "iteration: " << iteration << endl;

        // wait the rest of period (us)
        while (timer_step.end() < dt_controller*1000*1000);
    }

    cout << "Total time: " << timer_total.end()/1000 << " ms\n";

    cout << "[Main Control Thread]: thread end\n";
    return nullptr;
}

int main(int argc, char** argv)
{
    /*
    mlockall 锁定进程中所有映射到地址空间的页
    MCL_CURRENT 已经映射的进程地址，MCL_FUTURE 将来映射的进程地址
    */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
        cout << "mlockall failed: %m\n"; // 输入上一个函数的错误信息
        return -2;
    }
    
    // 主控制线程
    PeriodicRtTask *main_control_task = new PeriodicRtTask("[Main Control Thread]", 95, main_control_loop);
    sleep(1); 
    // 析构函数会join线程，等待子线程结束
    delete main_control_task;

    return 0;
}
