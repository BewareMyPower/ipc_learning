// mq_consumer.cc
#include <ctype.h>
#include <signal.h>
#include <sys/epoll.h>
#include "mq_protocol.h"
#include "mq_util.h"
#include "print_util.h"

static bool do_receive = true;

void HandleRead(int mqd, char* buffer, size_t buffer_size) {
    auto& message = *reinterpret_cast<Message*>(buffer);
    ssize_t n;
    while ( (n = mq_receive(mqd, buffer, buffer_size, nullptr)) >= 0) {
        printf("[Message id: %d] %d bytes:\n", message.id, message.length);
        HexDump(message.info, message.length);
    }
    if (errno != EAGAIN)
        errorExit(errno, "mq_receive");
}

int main(int argc, char* argv[]) {
    if (argc != 2)
        errorExit("Usage: %s mq-name", argv[0]);

    int mqd = MqOpen(argv[1], O_RDONLY | O_NONBLOCK);

    int epfd = epoll_create(1);
    if (epfd == -1)
        errorExit(errno, "epoll_create");

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = mqd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1)
        errorExit(errno, "epoll_ctl ADD");

    constexpr int timeout_ms = 1000;
    char buffer[MAX_MESSAGE_SIZE];

    signal(SIGINT, [](int) { do_receive = false; });
    while (do_receive) {
        int num_wait = epoll_wait(epfd, &ev, 1, timeout_ms);
        if (num_wait == -1) {
            if (errno == EINTR)
                continue;
            else
                errorExit("epoll_wait");
        }
        if (num_wait > 0)
            HandleRead(ev.data.fd, buffer, MAX_MESSAGE_SIZE);
    }

    printf("consumer stopped.\n");
    return 0;
}
