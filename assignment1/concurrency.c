#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "mt19937.c"

void setupSignalCatching();
void signalCatch( int signal );

void setupThreads();
void waitThreads();

int getRandomNumberRange( int floor, int ceil );

void* producerThread( void *params );
void* consumerThread( void *params );

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
    while(1) {

    }
};

void* consumerThread( void *params ) {
    while(1) {

    }
};

void waitThreads() {
    pthread_join( consumerThread1, NULL );
    pthread_join( consumerThread2, NULL );
    pthread_join( producerThread1, NULL );
};

int getRandomNumberRange( int floor, int ceil ) {
    
};
