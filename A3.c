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
}

int main(int argc, char *argv[]) {
    /* TODO:
     * 1. Check that exactly one argument was given (the number of students, n).
     *    If not, print a usage message and return 1.
     * 2. Convert it to a number with atoi. If n < 1, print an error and return 1.
     * 3. Seed the random numbers once: srand(time(NULL));
     * 4. Make an array of student IDs (1..n). Pass &ids[i] to each thread, not &i.
     * 5. Initialise all three semaphores to 0 and the mutex.
     * 6. Create the TA thread (ta_thread) and n student threads (student_thread).
     * 7. Wait for every student thread to finish (pthread_join).
     * 8. Stop the TA (pthread_cancel), then join it.
     * 9. Destroy the semaphores and the mutex, and return 0.
     */
    return 0;
}