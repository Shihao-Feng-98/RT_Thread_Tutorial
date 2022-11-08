/*
条件变量(配合互斥锁使用)
场景：两个线程有先后执行顺序
作用：当达到某个信号时，才解锁，运行下一个线程
用法：在需要锁的代码块前后加锁和解锁，且设置信号(同一个线程加锁，解锁)

生产者消费者模型
通过缓冲区隔离生产者和消费者，与二者直连相比，避免相互等待，提高运行效率。
生产快于消费，缓冲区满，撑死。
消费快于生产，缓冲区空，饿死。
*/

#include <pthread.h> // -lpthread
#include <unistd.h> // sleep
#include <iostream>
using namespace std;

// global variables
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t g_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t g_not_full = PTHREAD_COND_INITIALIZER;

#define CAPCITY 20
char stock[CAPCITY];
int size = 0;

void show(const char* who, const char* cond, char s)
{
    for (int i = 0; i < size; i++)
    {
        cout << stock[i];
    }
    cout << " " << cond << " " << s << "(" << who << ")\n";
}


// 生产者线程
void* producer_func(void* argc)
{
    const char* who = (char*)argc;
    for(;;)
    {
        pthread_mutex_lock(&g_mutex);
        while (size >= CAPCITY) // 仓库满了，不能再生产了
        {
            cout << "The warehouse is full, thread[" << who << "]should not be produced\n";
            // 等到仓库不满信号，再释放锁
            pthread_cond_wait(&g_not_full, &g_mutex); // 阻塞
            // 被唤醒后，重新获得锁
            cout << "The warehouse is not full, thread[" << who << "]can be produced\n";
        }
        char prod = 'A'+rand()%26;
        show(who, "<--", prod);
        stock[size++] = prod;
        
        pthread_cond_signal(&g_not_empty);
        pthread_mutex_unlock(&g_mutex);
        usleep ((rand () % 100) * 1000);
    }

    return nullptr;
}

// 消费者线程
void* customer_func(void* argc)
{
    const char* who = (char*)argc;
    for(;;)
    {
        pthread_mutex_lock(&g_mutex);
        while (size == 0) // 仓库空了，不能继续消费了
        {
            cout << "The warehouse is empty, thread[" << who << "]should not be consumed\n";
            // 等到仓库不空信号，再释放锁
            pthread_cond_wait(&g_not_empty, &g_mutex); // 阻塞
            // 被唤醒后，重新获得锁
            cout << "The warehouse is not empty, thread[" << who << "]can be consumed\n";
        }
        char prod = stock[--size];
        show(who, "-->", prod);

        pthread_cond_signal(&g_not_full);
        pthread_mutex_unlock(&g_mutex);
        usleep ((rand () % 100) * 1000);
    }

    return nullptr;
}

int main(int argc, char **argv)
{
    pthread_t pid[2], cid[2];

    pthread_create(&pid[0], NULL, producer_func, (void*)"producer_1");
    pthread_create(&pid[1], NULL, producer_func, (void*)"producer_2");

    pthread_create(&cid[0], NULL, customer_func, (void*)"customer_1");
    pthread_create(&cid[1], NULL, customer_func, (void*)"customer_2");

    getchar(); // 等待
    return 0;
}
