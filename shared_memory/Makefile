CC = g++
FLAGS = -g -std=c++11
LIBRT = -lrt
LIBPTHREAD = -lpthread
PROGS = pshm_create pshm_write pshm_read pshm_unlink
ROOT = ..

all: $(PROGS)

%: %.cc
	$(CC) $(FLAGS) -I$(ROOT) $< $(LIBRT) $(LIBPTHREAD) -o $@

clean:
	rm -rf a.out $(PROGS)
