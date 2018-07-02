CC = g++
FLAGS = -g -std=c++11
LIBRT = -lrt
LIBPTHREAD = -lpthread
PROGS = psem_create psem_unlink
ROOT = ..

all: $(PROGS)

%: %.cc
	$(CC) $(FLAGS) -I$(ROOT) $< $(LIBRT) $(LIBPTHREAD) -o $@

clean:
	rm -rf a.out $(PROGS)
