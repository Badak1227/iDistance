#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "bpTree.h"

#define K 3          // 클러스터 수
#define DELTA 1000   // 클러스터 간 간격

// 쓰레드에 전달할 인자 구조체
typedef struct thread_arg{
    unsigned int* seed;
    bp_tree* root;
} thread_arg;

sem_t rw_s;
sem_t w_s;
sem_t r_s;

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
void* insert(void* arg) {
    thread_arg* args = (thread_arg*) arg;

    for(int i=0; i<100; i++){

        int point = 0;
        double min_dist = 200;
        double iDistance = 0;

        double x = (double)xorshift(args->seed);
        double y = (double)xorshift(args->seed);

        for (int i = 0; i < 5; i++) {
            double dist = sqrt(pow((double)reference_point[i][0] - x, 2) + pow((double)reference_point[i][1] - y, 2));

            if (dist < min_dist) {
                point = i;
                min_dist = dist;
            }
        }

        iDistance = point * reference_range + min_dist;

        printf("thread(id): %lu, insert: %.2f\n", pthread_self(), iDistance);

        value num;
        num.iDist = iDistance;
        num.pos[0] = x;
        num.pos[1] = y;

        sem_wait(&w_s);
        insert_bp(num, args->root);
        sem_post(&buffer_s);
        sem_post(&w_s);


        sleep(0.1);
    }
    

    return NULL;
}

//cluster_id * delta + dist
void* read(void* arg) {
    thread_arg* args = (thread_arg*) arg;

    for(int i=0; i<100; i++){

        int point = 0;
        double min_dist = 200;
        double iDistance = 0;

        double x = (double)xorshift(args->seed);
        double y = (double)xorshift(args->seed);

        for (int i = 0; i < 5; i++) {
            double dist = sqrt(pow((double)reference_point[i][0] - x, 2) + pow((double)reference_point[i][1] - y, 2));

            if (dist < min_dist) {
                point = i;
                min_dist = dist;
            }
        }

        iDistance = point * reference_range + min_dist;

        printf("thread(id): %lu, insert: %.2f\n", pthread_self(), iDistance);

        value num;
        num.iDist = iDistance;
        num.pos[0] = x;
        num.pos[1] = y;


        
        sem_wait(&rw_s);
        if(buffer > 4 ) sem_wait(&w_s);
        else sem

        sem_post(&rw_s);

        sem_wait(&w_s);
        insert_bp(num, args->root);
        sem_post(&buffer_s);
        sem_post(&w_s);


        sleep(0.1);
    }
    

    return NULL;
}

int main(){
    sem_init(&rw_s, 0, 1);
    sem_init(&w_s, 0, 1);
    buffer = 0;

    int n;
    unsigned int seed[2];

    bp_tree* root = get_bp(8);

    scanf("%d", &n);
    
    seed[0] = (unsigned int)n;
    seed[1] = 42;

    printf("\nB+ Tree structure:\n");

    for(int i=0; i<n; i++){
        printf("\n%d - ", i + 1);
        insert(seed, root);

        
        print_bp(root);
    }

    free_bp(root);
}