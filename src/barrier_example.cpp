/*
没搞懂
*/

#include <pthread.h> // -lpthread
// #include <sys/mman.h> // mlockall(MCL_CURRENT|MCL_FUTURE)
#include <unistd.h> // sleep
#include <iostream>
using namespace std;
// // utils
// #include <periodic_rt_task.h>

// global variables
pthread_barrier_t barr;

// ======== Thread Function ========  
void* thread_func(void* argc)
{
    long x = (long)argc;
    x = (x+2)*3; // 6 9 12
    cout << pthread_self() << " run...\n";
    sleep(x);
    cout << pthread_self() << " weak...\n";
    pthread_barrier_wait(&barr);
    cout << pthread_self() << " go on...\n";

    return nullptr;
}


int main(int argc, char **argv)
{    
    // 初始化屏障
    pthread_barrier_init(&barr, NULL, 3);

    // 创建线程
    pthread_t ids[3];
    for (int i = 0; i < 3; i++)
    {
        pthread_create(&ids[i], NULL, thread_func, (void*)i);
    }

    getchar(); // 暂停

    // 销毁屏障
    pthread_barrier_destroy(&barr);

    return 0;
}
