CC = g++
FLAGS = -g -std=c++11
LIBRT = -lrt
LIBPTHREAD = -lpthread
PROGS = pmsg_create pmsg_unlink pmsg_getattr pmsg_send pmsg_receive sample \
		mq_notify_sig mq_notify_thread epoll_consumer
ROOT = ../include

all: $(PROGS)

mq_notify_thread: mq_notify_thread.cc
	$(CC) $(FLAGS) -I$(ROOT) $< $(LIBPTHREAD) $(LIBRT) -o $@

%: %.cc
	$(CC) $(FLAGS) -I$(ROOT) $< $(LIBRT) -o $@

clean:
	rm -rf a.out $(PROGS)
