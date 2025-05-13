#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int count = 0; // 缓冲区当前项目数
int in = 0;    // 生产者插入位置
int out = 0;   // 消费者取出位置

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

// 生产者函数
void* producer(void* arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        
        // 如果缓冲区满，等待
        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&cond_producer, &mutex);
        }
        
        // 生产项目
        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        
        printf("Produced: %d | Buffer size: %d\n", i, count);
        
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_consumer); // 通知消费者
        
        usleep(100000); // 模拟生产耗时(100ms)
    }
    return NULL;
}

// 消费者函数
void* consumer(void* arg) {
    for (int i = 1; i <= NUM_ITEMS; i++) {
        pthread_mutex_lock(&mutex);
        
        // 如果缓冲区空，等待
        while (count == 0) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }
        
        // 消费项目
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        
        printf("Consumed: %d | Buffer size: %d\n", item, count);
        
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_producer); // 通知生产者
        
        usleep(150000); // 模拟消费耗时(150ms)
    }
    return NULL;
}

int main() {
    pthread_t producer_thread, consumer_thread;
    
    // 创建线程
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);
    
    // 等待线程结束
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);
    
    printf("Production and consumption completed.\n");
    
    // 清理资源
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_producer);
    pthread_cond_destroy(&cond_consumer);
    
    return 0;
}
