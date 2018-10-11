// mq_notify_sig.cc
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>
#include "error_handler.h"

constexpr int NOTIFY_SIG = SIGUSR1;

int main(int argc, char* argv[]) {
  if (argc != 2 || strcmp(argv[1], "--help") == 0)
    errorExit("Usage: %s mq-name", argv[0]);

  auto mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
  if (mqd == -1) errorExit(errno, "mq_open");

  struct mq_attr attr;
  if (mq_getattr(mqd, &attr) == -1) errorExit(errno, "mq_getattr");

  char* buffer = new char[attr.mq_msgsize + 1];

  sigset_t block_mask;
  sigemptyset(&block_mask);
  sigaddset(&block_mask, NOTIFY_SIG);
  if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1)
    errorExit(errno, "sigprocmask");

  struct sigaction sa;
  sa.sa_flags = 0;
  sa.sa_handler = [](int) {};
  if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) errorExit(errno, "sigaction");

  struct sigevent sev;
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = NOTIFY_SIG;
  if (mq_notify(mqd, &sev) == -1) errorExit(errno, "mq_notify");

  sigset_t empty_mask;
  sigemptyset(&empty_mask);

  ssize_t num_read;
  while (true) {
    // 不使用pause()的原因是防止在while循环中其他代码执行时收到信号
    // 此时sigsuspend仍然会唤醒，但是pause则会因错过信号而阻塞
    sigsuspend(&empty_mask);
    printf("PID: %ld notified!\n", static_cast<long>(getpid()));

    // 重新注册该信号，否则下次调用sigsuspend时会永远阻塞
    if (mq_notify(mqd, &sev) == -1) errorExit(errno, "mq_notify");

    while ((num_read = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0) {
      buffer[num_read] = '\0';
      printf("Read %ld bytes: %s\n", static_cast<long>(num_read), buffer);
    }

    if (errno != EAGAIN) errorExit(errno, "mq_receive");
  }

  delete[] buffer;
  return 0;
}
