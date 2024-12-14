#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "bpTree.h"

// 쓰레드에 전달할 인자 구조체
typedef struct thread_arg{
    unsigned int* seed;
    bp_tree* root;
} thread_arg;

sem_t r_lock;
sem_t lock;
int reader;
int data_num;
int read_num;

int reference_range = 55;
int reference_point[5][2] = {
    {10, 50},
    {30, 50},
    {50, 50},
    {70, 50},
    {90, 50},
}; // {x, y}

unsigned int xorshift(unsigned int* seed){
    unsigned int x = seed[0];
    unsigned int y = seed[1];

    seed[0] = seed[1];

    // 비트 혼합
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    seed[1] ^= seed[1] + x;

    return (seed[1]) % 100;
}

//cluster_id * delta + dist
void* insert_th(void* arg) {
    thread_arg* args = (thread_arg*) arg;

    for(int i=0; i<100; i++){

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
        printf("\ninsert thread(id): %lu, insert: %.2f (%.0f, %.0f)\n", pthread_self(), iDistance, x, y);
        insert_bp(num, args->root);
        data_num++;
        sem_post(&lock);

        sleep(0.1);
    }
    

    return NULL;
}

//cluster_id * delta + dist
void* read_th(void* arg) {
    thread_arg* args = (thread_arg*) arg;

    for(int i=0; i<100; i++){

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

        printf("\nread thread(id): %lu, search: %.2f (%.0f, %.0f)\n", pthread_self(), iDistance, x, y);
        if(data_num != 0) search_bp(iDistance, args->root);
        else printf("No Data In IDistance\n");

        sem_wait(&r_lock);
        reader--;
        if(reader == 0) sem_post(&lock);
        sem_post(&r_lock);
    }
    

    return NULL;
}

int main(){
    int n;
    unsigned int seed[2];

    thread_arg args;
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

        root = get_bp(11);
        args.root = root;

        seed[0] = (unsigned int)n;
        seed[1] = 42;
        args.seed = seed;

        pthread_create(&writers[0], NULL, insert_th, &args);
        pthread_create(&writers[1], NULL, insert_th, &args);
        pthread_create(&readers[0], NULL, read_th, &args);
        pthread_create(&readers[1], NULL, read_th, &args);

        pthread_join(writers[0], NULL);
        pthread_join(writers[1], NULL);
        pthread_join(readers[0], NULL);
        pthread_join(readers[1], NULL);

        free_bp(root);

        sem_destroy(&r_lock);
        sem_destroy(&lock);

        printf("Test %d - data_num: %d, read_num: %d\n\n", i+1, data_num, read_num);
    }
}