/*
多线程控制任务的demo, 不进行线程同步
只进行开始的同步(barrier)，其余靠时钟定时
优势:整体的控制周期能提高
劣势:每个线程读取到的数据可能不是当前周期最新的数据,线程间形成差分
*/

#include <pthread.h> // -lpthread
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// utils
#include <C_timer.h>
#include <periodic_rt_task.h>

pthread_barrier_t g_barr_start; // 线程开始标志位
bool g_stop_all = false; // 线程停止标志位

// ======== Main Control Thread Function ========  
void* main_control_loop(void* argc)
{   
    CTimer timer_step, timer_total;
    const double dt = 0.002; // 2ms
    double time_since_run = 0.;
    int iteration_main = 0;
    
    pthread_barrier_wait(&g_barr_start);

    timer_total.reset();
    // run periodic task
    while(time_since_run < 1.){
        timer_step.reset();

        // 执行计算(读变量，写变量)
        usleep(800);

        ++iteration_main;
        time_since_run += dt;
        // wait the rest of period (us)
        while (timer_step.end() < dt*1000*1000);
    }
    g_stop_all = true;
    cout << "Main actual time: " << timer_total.end()/1000/1000 << " s\n";
    cout << "Main desired time: " << time_since_run << " s\n";
    cout << "Main iteration: " << iteration_main << endl;

    return nullptr;
}

// ======== FR Control Thread Function ========  
void* FR_control_loop(void* argc)
{
    CTimer timer_FR;
    const double dt = 0.002; 
    int iteration_FR = 0;

    pthread_barrier_wait(&g_barr_start);

    // run periodic task
    while(!g_stop_all){
        timer_FR.reset();
        
        // 执行收发(读变量，写变量)
        usleep(800);

        ++iteration_FR;
        // wait the rest of period (us)
        while (timer_FR.end() < dt*1000*1000);
    }
    cout << "FR iteration: " << iteration_FR << endl; 

    return nullptr;
}

// ======== FL Control Thread Function ========  
void* FL_control_loop(void* argc)
{
    CTimer timer_FL;
    const double dt = 0.002; 
    int iteration_FL = 0;

    pthread_barrier_wait(&g_barr_start);

    // run periodic task
    while(!g_stop_all){
        timer_FL.reset();
        
        // 执行收发(读变量，写变量)
        usleep(800);

        ++iteration_FL;
        // wait the rest of period (us)
        while (timer_FL.end() < dt*1000*1000);
    }
    cout << "FL iteration: " << iteration_FL << endl; 

    return nullptr;
}

// ======== RR Control Thread Function ========  
void* RR_control_loop(void* argc)
{
    CTimer timer_RR;
    const double dt = 0.002; 
    int iteration_RR = 0;

    pthread_barrier_wait(&g_barr_start);

    // run periodic task
    while(!g_stop_all){
        timer_RR.reset();
        
        // 执行收发(读变量，写变量)
        usleep(800);

        ++iteration_RR;
        // wait the rest of period (us)
        while (timer_RR.end() < dt*1000*1000);
    }
    cout << "RR iteration: " << iteration_RR << endl; 

    return nullptr;
}

// ======== RL Control Thread Function ========  
void* RL_control_loop(void* argc)
{
    CTimer timer_RL;
    const double dt = 0.002;
    int iteration_RL = 0;

    pthread_barrier_wait(&g_barr_start);

    // run periodic task
    while(!g_stop_all){
        timer_RL.reset();
        
        // 执行收发(读变量，写变量)
        usleep(800);

        ++iteration_RL;
        // wait the rest of period (us)
        while (timer_RL.end() < dt*1000*1000);
    }
    cout << "RL iteration: " << iteration_RL << endl; 

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

    // 初始化屏障
    pthread_barrier_init(&g_barr_start, NULL, 5);

    // 创建线程
    PeriodicRtTask *main_control_task = new PeriodicRtTask("[Main Control Thread]", 95, main_control_loop, 6);
    PeriodicRtTask *FR_control_task = new PeriodicRtTask("[FR Control Thread]", 95, FR_control_loop, 2);
    PeriodicRtTask *FL_control_task = new PeriodicRtTask("[FL Control Thread]", 95, FL_control_loop, 3);
    PeriodicRtTask *RR_control_task = new PeriodicRtTask("[RR Control Thread]", 95, RR_control_loop, 4);
    PeriodicRtTask *RL_control_task = new PeriodicRtTask("[RL Control Thread]", 95, RL_control_loop, 5);

    // 等待线程创建
    sleep(1); 

    // 合并线程
    delete RL_control_task;
    delete RR_control_task;
    delete FL_control_task;
    delete FR_control_task;
    delete main_control_task;

    return 0;
}
