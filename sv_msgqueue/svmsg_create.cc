// svmsg_create.cc
#include "sv_ipc.h"
#include <sys/msg.h>
#include <unistd.h>
#include "str_parse.h"

inline void usageError(const char* name, const char* msg) {
    if (msg)
        fprintf(stderr, "%s\n", msg);
    fprintf(stderr,
            "Usage: %s [-cx] {-f pathname | -k key | -p} [octal-perms]"
            "    -c           Use IPC_CREAT flag\n"
            "    -x           Use IPC_EXCL flag\n"
            "    -f pathname  Generate key using ftok()\n"
            "    -k key       Use 'key' as key\n"
            "    -p           Use IPC_PRIVATE key\n",
            name);
    exit(1);
}

static key_t key;
static int flags = 0;
static int perms;

static void parseArgs(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    parseArgs(argc, argv);

    auto msqid = msgget(key, flags | perms);
    if (msqid == -1)
        errorExit(errno, "msgget");

    printf("%d\n", msqid);
    return 0;
}

static void parseArgs(int argc, char* argv[]) {
    int numKeyFlags = 0;
    while (true) {
        int opt = getopt(argc, argv, "cf:k:px");
        if (opt == -1)
            break;

        switch (opt) {
        case 'c':
            flags |= IPC_CREAT;
            break;

        case 'f':
            key = Ftok(optarg, 1);
            numKeyFlags++;
            break;

        case 'k':
            key = StrToInt(optarg, 10);
            numKeyFlags++;
            break;

        case 'p':
            key = IPC_PRIVATE;
            numKeyFlags++;
            break;

        case 'x':
            flags |= IPC_EXCL;
            break;

        default:
            usageError(argv[0], "Bad option\n");
        }
    }

    if (numKeyFlags != 1)
        usageError(argv[0], "Exactly one of the options -f, -k, or "
                            "-p must be supplied");

    perms = (optind == argc) ? (S_IRUSR | S_IWUSR) : StrToInt(argv[optind], 8);
}
