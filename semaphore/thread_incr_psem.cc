// thread_incr_psem.cc
#include <semaphore.h>
#include <pthread.h>
#include "sem_util.h"

static int glob = 0;
static sem_t sem;

static void* threadFunc(void* arg) {  // 循环arg次来递增glob
    int loops = *static_cast<int*>(arg);
    for (int i = 0; i < loops; i++) {
        if (sem_wait(&sem) == -1)
            errorExit("sem_wait");

        int loc = glob;
        loc++;
        glob = loc;

        if (sem_post(&sem) == -1)
            errorExit("sem_post");
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    int loops = (argc > 1) ? strtol(argv[1], nullptr, 10) : 1000000;
    if (loops < 1)
        errorExit("%s must be > 0", argv[1]);

    if (sem_init(&sem, 0, 1) == -1)
        errorExit(errno, "sem_init");

#define NUM_THREADS 8
    pthread_t t[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        int error = pthread_create(&t[i], nullptr, threadFunc, &loops);
        if (error != 0)
            errorExit(error, "pthread_create");
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int error = pthread_join(t[i], nullptr);
        if (error != 0)
            errorExit(error, "pthread_join");
    }

    printf("glob = %d\n", glob);
    sem_destroy(&sem);
    return 0;
}
