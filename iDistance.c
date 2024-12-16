#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#include "bpTree.h"

// 쓰레드에 전달할 매개변수 구조체
typedef struct thread_arg{
    unsigned int* seed;
    bp_tree* root;
    int id;
} thread_arg;

sem_t r_lock; //read 락
sem_t lock; //공통 락
int reader; //실행되고 있는 reader 수 기록
int data_num; // insert 작업 횟수
int read_num; // read 작업 횟수

//쓰레드 성능 측정
double insert_response[2];
double read_response[2];

//입출력 성능 측정
double insert_time[2];
double read_time[2];

struct timespec sleep;

int reference_range = 55; //최대 레퍼런스 거리
int reference_point[5][2] = {
    {10, 50},
    {30, 50},
    {50, 50},
    {70, 50},
    {90, 50},
}; // 레퍼런스 포인트 5개

//xorshift를 이용한 고정 시드 생성성
unsigned int xorshift(unsigned int* seed){
    unsigned int x = seed[0];

    seed[0] = seed[1];

    // 비트 혼합
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    seed[1] ^= seed[1] + x;

    return (seed[1]) % 100;
}

//iDistance insert
void* insert_th(void* arg) {
    clock_t start_th, end_th;
    clock_t start_in, end_in;
    thread_arg* args = (thread_arg*) arg;

    start_th = clock();

    for(int i=0; i<100; i++){
        start_in = clock();
        int point = 0;
        double min_dist = 200;
        double iDistance = 0;

        sem_wait(&lock);
        double x = (double)xorshift(args->seed);
        double y = (double)xorshift(args->seed);
        sem_post(&lock);

        for (int i = 0; i < 5; i++) {
            double dist = sqrt(pow((double)reference_point[i][0] - x, 2) + pow((double)reference_point[i][1] - y, 2));

            if (dist < min_dist) {
                point = i;
                min_dist = dist;
            }
        }

        iDistance = point * reference_range + min_dist;

        value num;
        num.iDist = iDistance;
        num.pos[0] = x;
        num.pos[1] = y;

        sem_wait(&lock);
        printf("insert: %.2f (%.0f, %.0f)\n", iDistance, x, y);
        insert_bp(num, args->root);
        data_num++;
        sem_post(&lock);

        end_in = clock();

        insert_time[args->id] += ((double) (end_in - start_in)) / CLOCKS_PER_SEC;
        nanosleep(&sleep, NULL);
    }
    
    end_th = clock();

    insert_response[args->id] = ((double) (end_th - start_th)) / CLOCKS_PER_SEC;
    
    return NULL;
}

//iDistance read
void* read_th(void* arg) {
    clock_t start_th, end_th;
    clock_t start_re, end_re;
    thread_arg* args = (thread_arg*) arg;

    start_th = clock();

    for(int i=0; i<100; i++){
        start_re = clock();
        int point = 0;
        double min_dist = 200;
        double iDistance = 0;

        sem_wait(&lock);
        double x = (double)xorshift(args->seed);
        double y = (double)xorshift(args->seed);
        sem_post(&lock);

        for (int i = 0; i < 5; i++) {
            double dist = sqrt(pow((double)reference_point[i][0] - x, 2) + pow((double)reference_point[i][1] - y, 2));

            if (dist < min_dist) {
                point = i;
                min_dist = dist;
            }
        }

        iDistance = point * reference_range + min_dist;

        value num;
        num.iDist = iDistance;
        num.pos[0] = x;
        num.pos[1] = y;

        sem_wait(&r_lock);
        reader++;
        read_num++;
        if(reader == 1) sem_wait(&lock);
        sem_post(&r_lock);

        if(data_num != 0) search_bp(num, args->root);
        else printf("No Data In IDistance\n");

        sem_wait(&r_lock);
        reader--;
        if(reader == 0) sem_post(&lock);
        sem_post(&r_lock);

        end_re = clock();

        read_time[args->id] += ((double) (end_re - start_re)) / CLOCKS_PER_SEC;
        nanosleep(&sleep, NULL);
    }
    
    end_th = clock();
    read_response[args->id] = ((double) (end_th - start_th)) / CLOCKS_PER_SEC;
    

    return NULL;
}

int main(){
    sleep.tv_sec = 0;
    sleep.tv_nsec = 100000000L;

    int n;
    unsigned int seed[2];

    thread_arg args[4];
    bp_tree* root = NULL;
    pthread_t readers[2], writers[2];

    scanf("%d", &n);

    for(int i=0; i<n; i++){

        printf("Test: %d\n", i+1);

        sem_init(&r_lock, 0, 1);
        sem_init(&lock, 0, 1);
        reader = 0;

        data_num = 0;
        read_num = 0;

        insert_time[0] = 0;
        insert_time[1] = 0;
        read_time[0] = 0;
        read_time[1] = 0;

        root = get_bp(11);

        seed[0] = (unsigned int)n;
        seed[1] = 42;
        
        for(int k=0; k<4; k++){
            args[k].root = root;
            args[k].seed = seed;
            args[k].id = k % 2;
        }
        
        pthread_create(&writers[0], NULL, insert_th, &args[0]);
        pthread_create(&writers[1], NULL, insert_th, &args[1]);
        pthread_create(&readers[0], NULL, read_th, &args[2]);
        pthread_create(&readers[1], NULL, read_th, &args[3]);

        pthread_join(writers[0], NULL);
        pthread_join(writers[1], NULL);
        pthread_join(readers[0], NULL);
        pthread_join(readers[1], NULL);

        free_bp(root);

        sem_destroy(&r_lock);
        sem_destroy(&lock);

        printf("Test %d - data_num: %d, read_num: %d\n", i+1, data_num, read_num);
        printf("Average insert time: %f\n", (insert_time[0] + insert_time[1])/data_num);
        printf("Average read time: %f\n", (read_time[0] + read_time[1])/data_num);
        printf("insert thread1's response time: %f\n", insert_response[0]);
        printf("insert thread2's response time: %f\n", insert_response[1]);
        printf("read thread1's response time: %f\n", read_response[0]);
        printf("read thread2's response time: %f\n\n", read_response[1]);
        sleep.tv_sec = 5;
        nanosleep(&sleep, NULL); // 5초간 Test 결과를 볼 수 있도록
		sleep.tv_sec = 0;
    }
}