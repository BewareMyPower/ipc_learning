// mq_manager.cc: create/remove POSIX message queue
#include <unistd.h>
#include "mq_protocol.h"
#include "mq_util.h"

static const char* mq_name = nullptr;

inline void UsageError(const char* name, const char* extra = nullptr) {
    if (extra)
        fprintf(stderr, "%s\n", extra);
    const char* format =
        "Usage: %s [-cir] [-m maxmsg] mq-name\n"
        "    -c        Create queue\n"
        "    -i        Show queue's attribute\n"
        "    -m maxmsg Set maximum # of messages (default: 50)\n"
        "    -r        Remove queue\n";
    errorExit(format, name);
}

enum MQ_METHOD {
    MQ_NONE,
    MQ_CREATE = 1,
    MQ_INFO,
    MQ_REMOVE
};

int main(int argc, char* argv[]) {
    MQ_METHOD mq_method = MQ_NONE;

    bool do_set_attr = false;
    struct mq_attr attr;
    attr.mq_msgsize = MAX_MESSAGE_SIZE;
    attr.mq_maxmsg = 50;

    for (int opt; (opt = getopt(argc, argv, "cim:r")) != -1; ) {
        switch (opt) {
        case 'c':
            if (mq_method != MQ_NONE)
                UsageError(argv[0], "-c/-i/-r must be exclusive");
            mq_method = MQ_CREATE;
            break;

        case 'i':
            if (mq_method != MQ_NONE)
                UsageError(argv[0], "-c/-i/-r must be exclusive");
            mq_method = MQ_INFO;
            break;

        case 'r':
            if (mq_method != MQ_NONE)
                UsageError(argv[0], "-c/-i/-r must be exclusive");
            mq_method = MQ_REMOVE;
            break;

        case 'm':
            attr.mq_maxmsg = strtol(optarg, nullptr, 10);
            do_set_attr = true;
            break;

        default:
            UsageError(argv[0]);
        }
    }

    if (optind >= argc)
        UsageError(argv[0], "mq-name must be specified");
    const char* mq_name = argv[optind];

    mqd_t mqd;
    switch (mq_method) {
    case MQ_NONE:
        break;

    case MQ_CREATE:
        mqd = MqCreate(mq_name, O_EXCL, MQ_USER_RW_PERMS, attr);
        printf("Create mq: \"%s\" success.\n", mq_name);
        printf("    maxmsg:  %ld\n", static_cast<long>(attr.mq_maxmsg));
        printf("    msgsize: %ld\n", static_cast<long>(attr.mq_msgsize));
        break;

    case MQ_INFO:
        mqd = MqOpen(mq_name, 0);
        attr = MqGetAttr(mqd);
        printf("flags:   %s\n", (attr.mq_flags == 0) ? "0" : "NONBLOCK");
        printf("maxmsg:  %ld\n", static_cast<long>(attr.mq_maxmsg));
        printf("msgsize: %ld\n", static_cast<long>(attr.mq_msgsize));
        printf("curmsgs: %ld\n", static_cast<long>(attr.mq_curmsgs));
        break;

    case MQ_REMOVE:
        if (mq_unlink(mq_name))
            errorExit(errno, "mq_unlink");
        printf("Remove mq: \"%s\" success.\n", mq_name);
        break;

    default:
        fprintf(stderr, "Unknown mq_method\n");
        exit(1);
    }

    return 0;
}
