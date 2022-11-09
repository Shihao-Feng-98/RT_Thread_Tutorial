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

// 互斥锁 
pthread_mutex_t g_mutex_FR = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_mutex_FL = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_mutex_RR = PTHREAD_MUTEX_INITIALIZER;
// 控制计算完成，可以进行电机指令的收发
pthread_cond_t g_cond_ctrl_finished = PTHREAD_COND_INITIALIZER; 
// 四路电机指令收发完成，可以进行控制计算
pthread_cond_t g_cond_FR_finished = PTHREAD_COND_INITIALIZER; 
pthread_cond_t g_cond_FL_finished = PTHREAD_COND_INITIALIZER; 
pthread_cond_t g_cond_RR_finished = PTHREAD_COND_INITIALIZER; 

// 时间参数
const double g_dt = 0.00333; //3.33ms
double g_time_since_run = 0.;
// 标志位
bool g_stop_all = false; 
bool g_FR_finished = true;
bool g_FL_finished = true;
bool g_RR_finished = true;

Robot robot;
CTimer g_timer_total;

// 加锁 解锁 条件变量激活 10-20us
// TODO: 原子操作 

// ======== Main Control Thread Function ========  
void* main_control_loop(void* argc)
{   
    CTimer timer_step;
    int iteration = 0;
    
    g_timer_total.reset();
    // init 
    timer_step.reset();
    pthread_mutex_lock(&g_mutex_FR); 
    pthread_mutex_lock(&g_mutex_FL); 
    pthread_mutex_lock(&g_mutex_RR); 
    robot.control_task();
    // cout << "control_task: " << iteration++ << endl;
    pthread_cond_broadcast(&g_cond_ctrl_finished);
    g_FL_finished = false;
    g_FR_finished = false;
    g_RR_finished = false;
    pthread_mutex_unlock(&g_mutex_RR); 
    pthread_mutex_unlock(&g_mutex_FL); 
    pthread_mutex_unlock(&g_mutex_FR); 

    // run periodic task
    while(g_time_since_run < 1.){
        // 加锁
        pthread_mutex_lock(&g_mutex_FR); 
        pthread_mutex_lock(&g_mutex_FL); 
        pthread_mutex_lock(&g_mutex_RR); 

        while(!g_RR_finished){
            pthread_cond_wait(&g_cond_RR_finished, &g_mutex_RR);
        }
        while(!g_FL_finished){
            pthread_cond_wait(&g_cond_FL_finished, &g_mutex_FL);
        }
        while(!g_FR_finished){ // condition is false
            // 如果子线程未全部完成，则解锁并等待条件变量
            pthread_cond_wait(&g_cond_FR_finished, &g_mutex_FR);
        }

        g_time_since_run += g_dt;
        // wait the rest of period (us)
        cout << timer_step.end() << " us\n";
        while (timer_step.end() < g_dt*1000*1000);

        timer_step.reset();

        // 执行计算(读变量，写变量)
        robot.control_task();
        // cout << "control_task: " << ++iteration << endl;

        // 通知所有子线程执行
        pthread_cond_broadcast(&g_cond_ctrl_finished);
        g_FL_finished = false;
        g_FR_finished = false;
        g_RR_finished = false;

        // 解锁
        pthread_mutex_unlock(&g_mutex_RR); 
        pthread_mutex_unlock(&g_mutex_FL); 
        pthread_mutex_unlock(&g_mutex_FR); 
    }
    g_stop_all = true;

    return nullptr;
}

// ======== FR Control Thread Function ========  
void* FR_control_loop(void* argc)
{
    int iteration_FR = 0;
    // run periodic task
    while(!g_stop_all){
        // 加锁
        pthread_mutex_lock(&g_mutex_FR); 

        while(g_FR_finished){ // condition is false
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_FR);
        }

        // 执行收发(读变量，写变量)
        robot.motor_task_FR();
        // cout << "FR_task: " << ++iteration_FR << endl;

        // 通知主线程执行
        pthread_cond_signal(&g_cond_FR_finished); 
        g_FR_finished = true;

        // 解锁
        pthread_mutex_unlock(&g_mutex_FR); 
    }
    // double end = g_timer_total.end()/1000/1000;
    // cout << "FR actul time: " << end << " s\n"; 

    return nullptr;
}

// ======== FL Control Thread Function ========  
void* FL_control_loop(void* argc)
{
    int iteration_FL = 0;
    // run periodic task
    while(!g_stop_all){
        // 加锁
        pthread_mutex_lock(&g_mutex_FL); 

        while(g_FL_finished){
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_FL);
        }

        // 执行收发(读变量，写变量)
        robot.motor_task_FL();
        // cout << "FL_task: " << ++iteration_FL << endl;

        // 通知主线程执行
        pthread_cond_signal(&g_cond_FL_finished); 
        g_FL_finished = true;
        
        // 解锁
        pthread_mutex_unlock(&g_mutex_FL);
    }
    // double end = g_timer_total.end()/1000/1000;
    // cout << "FL actul time: " << end << " s\n"; 

    return nullptr;
}


// ======== RR Control Thread Function ========  
void* RR_control_loop(void* argc)
{
    int iteration_RR = 0;
    // run periodic task
    while(!g_stop_all){
        // 加锁
        pthread_mutex_lock(&g_mutex_RR); 

        while(g_RR_finished){
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_RR);
        }

        // 执行收发(读变量，写变量)
        robot.motor_task_RR();
        // cout << "FL_task: " << ++iteration_RR << endl;

        // 通知主线程执行
        pthread_cond_signal(&g_cond_RR_finished); 
        g_RR_finished = true;
        
        // 解锁
        pthread_mutex_unlock(&g_mutex_RR);
    }
    // double end = g_timer_total.end()/1000/1000;
    // cout << "RR actul time: " << end << " s\n"; 

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
    PeriodicRtTask *FR_control_task = new PeriodicRtTask("[FR Control Thread]", 95, FR_control_loop, 2);
    PeriodicRtTask *FL_control_task = new PeriodicRtTask("[FL Control Thread]", 95, FL_control_loop, 3);
    PeriodicRtTask *RR_control_task = new PeriodicRtTask("[RR Control Thread]", 95, RR_control_loop, 4);
    PeriodicRtTask *main_control_task = new PeriodicRtTask("[Main Control Thread]", 95, main_control_loop, 5);

    sleep(1); 

    // 析构函数会join线程，等待线程结束
    delete FR_control_task;
    delete FL_control_task;
    delete RR_control_task;
    delete main_control_task;

    cout << "Desired time: " << g_time_since_run << " s" << endl;

    return 0;
}
