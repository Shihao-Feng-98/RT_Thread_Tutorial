/*
多线程控制任务的demo
一个主线程(控制计算)，四个子线程(电机通讯)
主线程需要读取四个子线程的状态，然后计算，最后写指令
子线程需要读指令，然后收发，最后写状态
主线程和各个子线程分别是同步关系，子线程间是并行关系

主线程先执行,然后通过条件变量(阻塞)通知子线程执行
子线程执行完信号量-1,主线程通过判断信号量来阻塞等待,
直到信号量为0,主线程执行完即+4
*/

#include <pthread.h> // -lpthread
#include <semaphore.h> // 信号量
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// utils
#include <C_timer.h>
#include <periodic_rt_task.h>
#include <Robot.h> // for debug

// 信号量
sem_t g_sem;
// 互斥锁 
pthread_mutex_t g_mutex_FR = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_mutex_FL = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_mutex_RR = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t g_mutex_RL = PTHREAD_MUTEX_INITIALIZER;
// 控制计算完成，可以进行电机指令的收发
pthread_cond_t g_cond_ctrl_finished = PTHREAD_COND_INITIALIZER; 

// 标志位
bool g_stop_all = false; 
bool g_FR_finished = true;
bool g_FL_finished = true;
bool g_RR_finished = true;
bool g_RL_finished = true;

Robot robot;
CTimer g_timer_total;

// ======== Main Control Thread Function ========  
void* main_control_loop(void* argc)
{   
    CTimer timer_step;
    const double dt = 0.00333; // 3.33ms
    double time_since_run = 0.;
    int iteration = 0;
    int sval; 
    
    g_timer_total.reset();
    // init
    timer_step.reset();

    pthread_mutex_lock(&g_mutex_FR); 
    pthread_mutex_lock(&g_mutex_FL); 
    pthread_mutex_lock(&g_mutex_RR); 
    pthread_mutex_lock(&g_mutex_RL); 
    
    robot.control_task();
    ++iteration;
    // cout << "control_task: " << ++iteration << endl;
    
    pthread_cond_broadcast(&g_cond_ctrl_finished);
    g_FL_finished = false;
    g_FR_finished = false;
    g_RR_finished = false;
    g_RL_finished = false;
    pthread_mutex_unlock(&g_mutex_RL); 
    pthread_mutex_unlock(&g_mutex_RR); 
    pthread_mutex_unlock(&g_mutex_FL); 
    pthread_mutex_unlock(&g_mutex_FR); 

    // run periodic task
    while(time_since_run < 5.){
        // wait for child threads
        while(1){
            sem_getvalue(&g_sem, &sval);
            if (sval == 0) {break;}
        }

        ++iteration;
        time_since_run += dt;
        // wait the rest of period (us)
        while (timer_step.end() < dt*1000*1000);

        timer_step.reset();

        pthread_mutex_lock(&g_mutex_FR); 
        pthread_mutex_lock(&g_mutex_FL); 
        pthread_mutex_lock(&g_mutex_RR); 
        pthread_mutex_lock(&g_mutex_RL); 

        // run task
        robot.control_task();
        // cout << "control_task: " << ++iteration << endl;

        // 通知所有子线程执行
        pthread_cond_broadcast(&g_cond_ctrl_finished);
        // flag reset
        g_FL_finished = false;
        g_FR_finished = false;
        g_RR_finished = false;
        g_RL_finished = false;
        // sem reset
        sem_post(&g_sem); // +1
        sem_post(&g_sem); // +1
        sem_post(&g_sem); // +1
        sem_post(&g_sem); // +1

        pthread_mutex_unlock(&g_mutex_RL); 
        pthread_mutex_unlock(&g_mutex_RR); 
        pthread_mutex_unlock(&g_mutex_FL); 
        pthread_mutex_unlock(&g_mutex_FR); 
    }
    g_stop_all = true;

    cout << "Desired time: " << time_since_run << " s" << endl;

    return nullptr;
}

// ======== FR Control Thread Function ========  
void* FR_control_loop(void* argc)
{
    int iteration_FR = 0;

    // run periodic task
    while(!g_stop_all){
        pthread_mutex_lock(&g_mutex_FR); 

        // wait for condition
        while(g_FR_finished){ 
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_FR);
        }

        // run task
        robot.motor_task_FR();
        // cout << "FR_task: " << ++iteration_FR << endl;

        // flag reset
        g_FR_finished = true;

        sem_wait(&g_sem); // -1

        pthread_mutex_unlock(&g_mutex_FR); 
    }
    cout << "FR actual time: " << g_timer_total.end()/1000/1000 << " s\n";
    return nullptr;
}

// ======== FL Control Thread Function ========  
void* FL_control_loop(void* argc)
{
    int iteration_FL = 0;

    while(!g_stop_all){
        pthread_mutex_lock(&g_mutex_FL); 

        while(g_FL_finished){
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_FL);
        }

        robot.motor_task_FL();
        // cout << "FL_task: " << ++iteration_FL << endl;
 
        g_FL_finished = true;

        sem_wait(&g_sem); // -1
        
        pthread_mutex_unlock(&g_mutex_FL);
    }
    cout << "FL actual time: " << g_timer_total.end()/1000/1000 << " s\n";
    return nullptr;
}


// ======== RR Control Thread Function ========  
void* RR_control_loop(void* argc)
{
    int iteration_RR = 0;

    while(!g_stop_all){
        pthread_mutex_lock(&g_mutex_RR); 

        while(g_RR_finished){
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_RR);
        }

        robot.motor_task_RR();
        // cout << "RR_task: " << ++iteration_RR << endl;

        g_RR_finished = true;
        
        sem_wait(&g_sem); // -1

        pthread_mutex_unlock(&g_mutex_RR);
    }
    cout << "RR actual time: " << g_timer_total.end()/1000/1000 << " s\n";
    return nullptr;
}

// ======== RL Control Thread Function ========  
void* RL_control_loop(void* argc)
{
    int iteration_RL = 0;

    while(!g_stop_all){
        pthread_mutex_lock(&g_mutex_RL); 

        while(g_RL_finished){
            pthread_cond_wait(&g_cond_ctrl_finished, &g_mutex_RL);
        }

        robot.motor_task_RL();
        // cout << "RL_task: " << ++iteration_RL << endl;

        g_RL_finished = true;
        
        sem_wait(&g_sem); // -1

        pthread_mutex_unlock(&g_mutex_RL);
    }
    cout << "RL actual time: " << g_timer_total.end()/1000/1000 << " s\n";
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

    sem_init(&g_sem, 0, 4);

    // creat threads
    PeriodicRtTask *FR_control_task = new PeriodicRtTask("[FR Control Thread]", 95, FR_control_loop, 2);
    PeriodicRtTask *FL_control_task = new PeriodicRtTask("[FL Control Thread]", 95, FL_control_loop, 3);
    PeriodicRtTask *RR_control_task = new PeriodicRtTask("[RR Control Thread]", 95, RR_control_loop, 4);
    PeriodicRtTask *RL_control_task = new PeriodicRtTask("[RL Control Thread]", 95, RL_control_loop, 5);
    PeriodicRtTask *main_control_task = new PeriodicRtTask("[Main Control Thread]", 95, main_control_loop, 6);

    sleep(1); 

    // join threads
    delete FR_control_task;
    delete FL_control_task;
    delete RR_control_task;
    delete RL_control_task;
    delete main_control_task;

    sem_destroy(&g_sem);

    return 0;
}
