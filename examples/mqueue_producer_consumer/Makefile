CXXFLAGS = -std=c++11 -g
ROOT = ../..
INCLUDE = $(ROOT)/include
LIBRT = -lrt
PROGS = mq_producer mq_consumer mq_manager

all: $(PROGS)

%: %.cc mq_protocol.h mq_util.h
	g++ $(CXXFLAGS) -I $(INCLUDE) $< $(LIBRT) -o $@

mq_consumer: mq_consumer.cc mq_protocol.h mq_util.h print_util.h
	g++ $(CXXFLAGS) -I $(INCLUDE) $< $(LIBRT) -o $@

clean:
	rm -rf $(PROGS) a.out
