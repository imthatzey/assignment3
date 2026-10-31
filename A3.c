#include <pthread.h>    // pthread_t, pthread_mutex_t, pthread_create, ...
#include <semaphore.h>  // sem_t, sem_init, sem_wait, sem_post
#include <stdio.h>      // printf
#include <stdlib.h>     // rand, atoi
#include <unistd.h>     // sleep


#define NUM_CHAIRS 3

int waiting_students = 0;        // students sitting in the hallway
pthread_mutex_t chair_mutex;     // protects waiting_students
sem_t students_sem;              // students signal the TA (wakes the TA if it's napping)
sem_t ta_sem;                    // the TA signals the next waiting student to come in
