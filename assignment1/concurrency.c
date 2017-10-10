/*
Authors:        Alan Neads & David Corbelli
Course:         cs444 f17       
References: Assembly code used to determine if CPU supports rdrand is referenced in-part from 
                        https://codereview.stackexchange.com/questions/147656/checking-if-cpu-supports-rdrand 

*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <limits.h>

#include "mt19937ar.c"

#define bit_RDRND   (1 << 30)

void setupSignalCatching();
void signalCatch( int signal );

void setupThreads();
void waitThreads();

int getRandomNumberRange( int floor, int ceil );

void* producerThread( void *params );
void* consumerThread( void *params );

int checkRand();
#define _rdrand_generate(x) ({ unsigned char err; asm volatile("rdrand %0; setc %1":"=r"(*x), "=qm"(err)); err;})

struct bufferItem {
    int number;
    int sleepDuration;
};

struct bufferItem bufferJobList[32];
pthread_mutex_t bufferJobListLock;

int producerJobListIndex;
int consumerJobListIndex;

pthread_t producerThread1;
pthread_t consumerThread1, consumerThread2;

pthread_cond_t producerCondition;
pthread_cond_t consumerCondition;

int main( int argc, char** argv ) {
    producerJobListIndex = 0;
    consumerJobListIndex = 0;
    init_genrand(time(NULL));
    
    pthread_mutex_init( &bufferJobListLock, NULL );

    setupSignalCatching();
    setupThreads();
    waitThreads();
   
    return 0;
};

void setupSignalCatching() {
    struct sigaction sigAction;
    
    sigemptyset( &sigAction.sa_mask );
    sigAction.sa_handler = signalCatch;
    sigaction( SIGINT, &sigAction, NULL );
};

void signalCatch( int signal ) {
    printf( "Caught signal: %d\n", signal );

    if ( signal == SIGINT ) {
        printf( "Exiting program from SIGINT\n" );
        exit(0);
    }    
};

void setupThreads() {
    pthread_cond_init( &producerCondition, NULL );
    pthread_cond_init( &consumerCondition, NULL );

    pthread_create( &producerThread1, NULL, producerThread, NULL );
    pthread_create( &consumerThread1, NULL, consumerThread, NULL );
    pthread_create( &consumerThread2, NULL, consumerThread, NULL );
};

void* producerThread( void *params ) {
    int producerSleepDuration;
    struct bufferItem newItem;
    pid_t pid = syscall(SYS_gettid);
    
    while(1) {
        pthread_mutex_lock(&bufferJobListLock);
        
        newItem.number = getRandomNumberRange( 0, INT_MAX - 1 );
        newItem.sleepDuration = getRandomNumberRange( 2, 9 );
        producerSleepDuration = getRandomNumberRange( 3, 7 );

        while( producerJobListIndex >= 31 ) {
            pthread_cond_wait( &producerCondition, &bufferJobListLock );
        }

        bufferJobList[producerJobListIndex] = newItem;
        producerJobListIndex += 1;

        if ( producerJobListIndex > 31 ) {
            producerJobListIndex = 0;
        }

        pthread_cond_signal(&consumerCondition);
        pthread_mutex_unlock(&bufferJobListLock);

        printf("[PRODUCER][PID %lu] Produced number %d to the queue, will sleep for %d seconds.\n", pid, newItem.number, producerSleepDuration );
        
        sleep(producerSleepDuration);
    }
};

void* consumerThread( void *params ) {
    int number;
    int sleepDuration;
    pid_t pid = syscall(SYS_gettid);

    while(1) {
        pthread_mutex_lock(&bufferJobListLock);

        while( producerJobListIndex == 0 || producerJobListIndex == consumerJobListIndex ) {
            pthread_cond_wait( &consumerCondition, &bufferJobListLock );
        }

        number = bufferJobList[consumerJobListIndex].number;
        sleepDuration = bufferJobList[consumerJobListIndex].sleepDuration;

        consumerJobListIndex += 1;
        if ( consumerJobListIndex > 31 ) {
            consumerJobListIndex = 0;
        }
        pthread_cond_signal(&producerCondition);
        pthread_mutex_unlock(&bufferJobListLock);

        printf("[CONSUMER][PID %lu] Consumed number %d, will sleep for %d seconds.\n", pid, number, sleepDuration );

        sleep(sleepDuration);
    }
};

void waitThreads() {
    pthread_join( consumerThread1, NULL );
    pthread_join( consumerThread2, NULL );
    pthread_join( producerThread1, NULL );
};

int getRandomNumberRange( int floor, int ceil ) {
    int num = 0;
    if(checkRand() == 0){
        _rdrand_generate(&num);
    }
    else{
        num = abs(genrand_int32());
    }
    num = floor + num % (ceil + 1 - floor);
    return num;
};

int checkRand() {
    int eax;
    int ecx;
    eax = 0x01;
    __asm__ __volatile__("cpuid;": "=a"(eax), "=c"(ecx) : "a"(eax));
                                                 
         
    if(ecx & bit_RDRND){
        //use rdrand
        return 0;
    }
    else{
        //use mt19937
        return 1;
    }
        
};
