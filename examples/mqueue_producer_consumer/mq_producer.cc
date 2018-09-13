// mq_producer.cc
#include <signal.h>
#include "mq_protocol.h"
#include "mq_util.h"

int main(int argc, char* argv[]) {
    if (argc != 2)
        errorExit("Usage: %s mq-name", argv[0]);

    int mqd = MqOpen(argv[1], O_WRONLY);

    char buffer[MAX_MESSAGE_SIZE + 1];  // '\0'
    auto& message = *reinterpret_cast<Message*>(&buffer[0]);
    message.id = 0;

    signal(SIGINT, [](int) { fclose(stdin); });
    while (fgets(message.info, MAX_INFO_LENGTH + 1, stdin)) {
        // ignore empty line
        size_t len = strlen(message.info);
        if (len > 0 && message.info[len - 1] == '\n')
            len--;
        if (len == 0)
            continue;

        message.id++;
        message.length = len;
        if (mq_send(mqd, buffer, MESSAGE_HEADER_SIZE + len, 0) == -1) {
            perror("mq_send");
            break;
        }
        bzero(message.info, sizeof(buffer) - MESSAGE_HEADER_SIZE);
    }

    printf("producer stopped.\n");
    return 0;
}
