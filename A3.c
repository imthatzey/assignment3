/*
 * Compile: gcc -Wall -pthread A3.c -o A3
 * Run:     ./A3 <num_students>     e.g. ./A3 5
 */

#include <pthread.h>    // pthread_t, pthread_mutex_t, pthread_create, ...
#include <semaphore.h>  // sem_t, sem_init, sem_wait, sem_post
#include <stdio.h>      // printf
#include <stdlib.h>     // rand, atoi
#include <time.h>       // time (to seed rand)
#include <unistd.h>     // sleep


#define NUM_CHAIRS 3
#define HELPS_NEEDED   3      // how many times each student needs help before finishing
#define MAX_PROGRAM_MS 3000   // longest a student programs before asking for help (ms)
#define MAX_HELP_MS    2000   // longest one help session lasts (ms)

/* Sleep for a random time between 1 and max_ms milliseconds. */
void random_sleep(int max_ms) {
    usleep((rand() % max_ms + 1) * 1000);   // usleep takes microseconds
}

int waiting_students = 0;        // students sitting in the hallway
pthread_mutex_t chair_mutex;     // protects waiting_students
sem_t students_sem;              // students signal the TA (wakes the TA if it's napping)
sem_t ta_sem;                    // the TA signals the next waiting student to come in
sem_t help_done_sem;             // the TA signals the student that help is finished

void *student_thread(void *arg) {
    int id = *(int *)arg;
    int helped = 0;                          // times I actually got help

    while (helped < HELPS_NEEDED) {
        printf("Student %d is programming.\n", id);
        random_sleep(MAX_PROGRAM_MS);        // programming

        pthread_mutex_lock(&chair_mutex);
        if (waiting_students < NUM_CHAIRS) {
            waiting_students++;                       // take a chair
            printf("Student %d takes a seat in the hallway (%d chairs left).\n",
                   id, NUM_CHAIRS - waiting_students);
            pthread_mutex_unlock(&chair_mutex);

            sem_post(&students_sem);         // ring the doorbell
            sem_wait(&ta_sem);         // wait to be called
            printf("Student %d is getting help from the TA.\n", id);
            sem_wait(&help_done_sem);        // getting help until the TA says it's done
            helped++;                        // only counts if I got help
        } else {
            pthread_mutex_unlock(&chair_mutex);    // no chair: back to programming
            printf("Student %d found no free chair, will come back later.\n", id);
        }
    }
    printf("Student %d is done and leaves.\n", id);
    
    //pthread_exit?
    return NULL;
}

void *ta_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&chair_mutex);
        if (waiting_students == 0)
            printf("TA is napping.\n");
        pthread_mutex_unlock(&chair_mutex);

        sem_wait(&students_sem);               // nap here if nobody is waiting

        pthread_mutex_lock(&chair_mutex);
        waiting_students--;                    // one chair frees up
        printf("TA is helping a student (%d still waiting).\n", waiting_students);
        pthread_mutex_unlock(&chair_mutex);

        sem_post(&ta_sem);                     // "next!" - call one waiting student

        random_sleep(MAX_HELP_MS);             // helping them
        sem_post(&help_done_sem);              // tell the student help is finished
    }
    
    //pthread_exit?
}

int main(int argc, char *argv[]) {
    // Check that exactly one argument was given (the number of students, n). If not, print a usage message and return 1.
    if (argc != 2) {
        printf("You did not provide the right arguments. Please only enter the number of students. \n");
        return 1;
    }


    // convert the input to a number, if it's not valid print an error
    int n_students = atoi(argv[1]);
    if (n_students < 1){
        printf("The number of students cannot be less than 1. \n");
        return 1;
    }


    // seed the random numbers
    srand(time(NULL));
    

    // initialise all three semaphores to 0 and the mutex
    if (sem_init(&students_sem, 0, 1) != 0){
        printf("Error in initializing semaphore \n");
        return 1;
    }
    
    if (sem_init(&ta_sem, 0, 1) != 0){
        printf("Error in initializing semaphore \n");
        return 1;
    }
    
    if (sem_init(&help_done_sem, 0, 1) != 0){
        printf("Error in initializing semaphore \n");
        return 1;
    }
    
    if (pthread_mutex_init (&chair_mutex, NULL) != 0){
        printf("Error in initializing mutex \n");
        return 1;
    }
    
    // create the TA thread and student threads
    pthread_t tid[1 + n_students];
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    
    if (pthread_create(&tid[0], &attr, ta_thread, argv[1]) != 0){
        printf("Error creating ta_thread \n");
        return 1;
    }
    
    for(int i = 1; i < n_students; i++){
        if (pthread_create(&tid[i], &attr, student_thread, argv[1]) != 0){
            printf("Error creating student_thread %d \n", i);
            return 1;
        }
    }
    
    // wait for every student thread to finish
    for (int i = 1; i < n_students + 1; i++){
        pthread_join(tid[i], NULL);
    }
    
    
    // stop the TA, then join it.
    if (pthread_cancel(tid[0]) != 0){
        printf("Unable to cancel ta_thread \n");
        return 1;
    }
    
    pthread_join(tid[0], NULL);
    
    
    // destroy the semaphores and the mutex, and return 0.
    sem_destroy(&students_sem);
    sem_destroy(&ta_sem);
    sem_destroy(&help_done_sem);

    pthread_mutex_destroy(&chair_mutex);
    
     
    return 0;
}
