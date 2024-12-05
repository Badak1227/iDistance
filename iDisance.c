#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define K 3          // 클러스터 수
#define DELTA 1000   // 클러스터 간 간격

sem_t s;
sem_t buffer;

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
void* insert(void* arg){
    int point = 0;
    double min_dist = 200;
    double iDistance = 0;

    unsigned int* seed = (unsigned int*)arg;

    double x = (double)xorshift(seed);
    double y = (double)xorshift(seed);

    for(int i=0; i<5; i++){
        int dist = sqrt(pow((double)reference_point[i][0] - x, 2) + pow((double)reference_point[i][1] - y, 2));
        
        if(dist < min_dist){
            point = i;
            min_dist = dist;
        }
    }

    iDistance = point * reference_range + min_dist;

    return NULL;
}

void* read(void* arg){
    sem_wait(&s);
}

int main(){
    sem_init(&s, 0, 1);
    pthread_t thread[4];

    int n;
    unsigned int seed[2];

    scanf("%d", &n);
    
    seed[0] = (unsigned int)n;
    seed[1] = 42;
.
    for(int i=0; i<n; i++){
        pthread_create(&thread[0], NULL, insert, (void*)seed);
        pthread_create(&thread[1], NULL, insert, (void*)seed);
        pthread_create(&thread[2], NULL, read, (void*)seed);
        pthread_create(&thread[3], NULL, read, (void*)seed);
    }
}