/*
多线程控制任务的demo
一个主线程(控制计算)，四个子线程(电机通讯)
主线程需要读取四个子线程的状态，然后计算，最后写指令
子线程需要读指令，然后收发，最后写状态
此时需要用到条件变量和互斥锁进行线程同步
主线程和各个子线程是同步关系，子线程间是并行关系
*/

#include <pthread.h> // -lpthread
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// utils
#include <C_timer.h>
#include <periodic_rt_task.h>
#include <Robot.h> // for debug


pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
// 控制计算完成，可以进行电机指令的收发
pthread_cond_t g_cond_ctrl_finished = PTHREAD_COND_INITIALIZER; 
// 四路电机指令收发完成，可以进行控制计算
pthread_cond_t g_cond_sr_finished = PTHREAD_COND_INITIALIZER; 

// 时间参数
const double g_dt = 0.00333; //3.33ms
double g_time_since_run = 0.;
// int g_iteration = 0;
// 标志位
bool g_stop_all = false; 
bool g_ctrl_finished = false;
bool g_FR_finished = false;

Robot robot;
CTimer g_timer_total;

// 加锁 解锁 条件变量激活 10-20us

// ======== Main Control Thread Function ========  
void* main_control_loop(void* argc)
{   
    CTimer timer_step;
    
    g_timer_total.reset();
    // init 
    timer_step.reset();
    pthread_mutex_lock(&g_mutex); 
    robot.control_task();
    // cout << "control_task: " << g_iteration << endl;
    pthread_mutex_unlock(&g_mutex); 
    pthread_cond_broadcast(&g_cond_ctrl_finished);
    g_ctrl_finished = true; 

    // run periodic task
    while(g_time_since_run < 5.){
        // 加锁
        pthread_mutex_lock(&g_mutex); 

        while(!g_FR_finished){
            // 如果子线程未完成，则解锁并等待条件变量
            pthread_cond_wait(&g_cond_sr_finished, &g_mutex);
        }
        g_time_since_run += g_dt;
        // g_iteration++;
        // wait the rest of period (us)
        // cout << timer_step.end() << " us\n";
        while (timer_step.end() < g_dt*1000*1000);

        timer_step.reset();

        // 执行计算(读变量，写变量)
        robot.control_task();
        // cout << "control_task: " << g_iteration << endl;

        // 解锁
        pthread_mutex_unlock(&g_mutex); 
        // 通知所有子线程执行
        pthread_cond_broadcast(&g_cond_ctrl_finished);

        g_ctrl_finished = true; 
        g_FR_finished = false; 
    }
    g_stop_all = true;

    return nullptr;
}

// ======== FR Control Thread Function ========  
void* FR_control_loop(void* argc)
{
    // run periodic task
    while(!g_stop_all){
        // 加锁
        pthread_mutex_lock(&g_mutex); 

        while(!g_ctrl_finished){
            // 如果主线程未完成，则解锁并等待条件变量
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex);
        }

        // 执行收发(读变量，写变量)
        robot.motor_task();
        // cout << "motor_task: " << g_iteration << endl;

        // 解锁
        pthread_mutex_unlock(&g_mutex); 
        // 通知主线程执行
        pthread_cond_broadcast(&g_cond_sr_finished); 
        g_FR_finished = true; 
        g_ctrl_finished = false; 
    }
    cout << "desired time: " << g_time_since_run << " s" << endl;
    cout << "actul time: " << g_timer_total.end()/1000/1000 << " s\n"; 

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
    PeriodicRtTask *FR_control_task = new PeriodicRtTask("[FR Control Thread]", 95, FR_control_loop, 1);
    PeriodicRtTask *main_control_task = new PeriodicRtTask("[Main Control Thread]", 95, main_control_loop, 0);

    sleep(1); 

    // 析构函数会join线程，等待线程结束
    delete FR_control_task;
    delete main_control_task;

    return 0;
}
