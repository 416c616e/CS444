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

#include "mt19937ar.c"

#define bit_RDRND   (1 << 30)

void setupSignalCatching();
void signalCatch( int signal );

void setupThreads();
void waitThreads();

int getRandomNumberRange( int floor, int ceil, int randGen );

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
    int randGen;
    int output;
    // pthread_mutex_init( &bufferJobListLock, NULL );

    //  setupSignalCatching();
    //  setupThreads();

    // waitThreads();
    randGen = checkRand();
    output = getRandomNumberRange(2, 9, randGen);
   
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

int getRandomNumberRange( int floor, int ceil, int randGen ) {
    int num = 0;
    int num2 = 0;
    if(randGen == 0){
        printf("using rdrand");
        _rdrand_generate(&num);
    }
    else{
        printf("using Mersene Twister");
        num = abs(genrand_int32());
        num2 = abs(genrand_int32());
        //printf("%d\n", num);
        //printf("%d\n", num2);
    }
    num = floor + num % (ceil + 1 - floor);
    num2 = floor + num % (ceil + 1 - floor);
    printf("%d\n", num);
    printf("%d\n", num2);
    return num;
};

int checkRand() {
    int eax;
    int ecx;
    eax = 0x01;
    __asm__ __volatile__("cpuid;": "=a"(eax), "=c"(ecx) : "a"(eax));
                                                 
    printf("The value of the ecx register is %08x.\n", ecx);
         
    if(ecx & bit_RDRND){
        //use rdrand
        printf("using rdrand\n");
        return 0;
    }
    else{
        //use mt19937
        printf("use mt19937");
        return 1;
    }
        
};
