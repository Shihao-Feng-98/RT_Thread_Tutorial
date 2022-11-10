/*
Example for single real-time thread
*/

#include <pthread.h> // -lpthread
#include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// utils
#include <C_timer.h>
#include <periodic_rt_task.h>
#include <Robot.h>

// ======== Main Control Thread Function ========  
void* main_control_loop(void* argc)
{
    CTimer timer_step, timer_total;
    Robot robot;

    double dt = 0.00333; // 3.33ms
    double time_since_run = 0.; 
    int iteration = 0;
    // cout << "[Main Control Thread]: thread start\n";

    timer_total.reset();
    // run periodic task
    while(time_since_run < 5.)
    {
        timer_step.reset();
        
        // do something 
        robot.control_task(); 
        robot.motor_task_FR(); 

        time_since_run += dt;
        ++iteration;
        cout << timer_step.end() << " us" << endl;
        // wait the rest of period (us)
        while (timer_step.end() < dt*1000*1000);
    }
    cout << "Actual time: " << timer_total.end()/1000 << " ms\n";
    cout << "Desired time: " << time_since_run*1000 << " ms\n";
    cout << "Iteration: " << iteration << endl;

    // cout << "[Main Control Thread]: thread end\n";
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
