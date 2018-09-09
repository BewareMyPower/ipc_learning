// mq_notify_thread.cc
#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include "error_handler.hpp"

static void notifySetup(mqd_t& mqd);  // 向消息队列中注册通知线程

static void notifyFunc(union sigval sv) {
    // 消息队列描述符的地址存于sv中
    auto& mqd = *reinterpret_cast<mqd_t*>(sv.sival_ptr);

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        errorExit(errno, "mq_getattr");

    char* buffer = new char[attr.mq_msgsize + 1];

    notifySetup(mqd);

    ssize_t num_read;
    while ((num_read = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0)
        printf("Read %ld bytes\n", static_cast<long>(num_read));
    if (errno != EAGAIN) errorExit(errno, "mq_receive");

    delete[] buffer;
    pthread_exit(NULL);
}

static void notifySetup(mqd_t& mqd) {
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = notifyFunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &mqd;

    if (mq_notify(mqd, &sev) == -1)
        errorExit(errno, "mq_notify");
}

int main(int argc, char* argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        errorExit("Usage: %s mq_name", argv[0]);

    auto mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == -1) errorExit("mq_open");

    notifySetup(mqd);
    pause();  // 主线程永远中止，因为定时器通知是在一个单独的线程中调用notifyFunc()来分发的
    return 0;
}
/*
# ps -T -o pid,tid,args,wchan | grep mq_notify_thread
 2372  2372 ./mq_notify_thread /mq      pause
 2372  2373 ./mq_notify_thread /mq      skb_recv_datagram
*/
