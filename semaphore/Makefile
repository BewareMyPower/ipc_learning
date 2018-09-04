CC = g++
FLAGS = -g -std=c++11
LIBRT = -lrt
LIBPTHREAD = -lpthread
PROGS = psem_create psem_unlink psem_wait psem_post psem_getvalue
ROOT = ..

all: $(PROGS)

%: %.cc
	$(CC) $(FLAGS) -I$(ROOT) $< $(LIBRT) $(LIBPTHREAD) -o $@

clean:
	rm -rf a.out $(PROGS)
