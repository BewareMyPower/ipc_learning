CC = g++
FLAGS = -g -std=c++11
LIBRT = -lrt
LIBPTHREAD = -lpthread
PROGS = svmsg_create
ROOT = ../include

all: $(PROGS)

%: %.cc
	$(CC) $(FLAGS) -I$(ROOT) $< $(LIBRT) -o $@

clean:
	rm -rf a.out $(PROGS)
