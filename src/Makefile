
CXXFLAGS= -g
LIBS= -lcv -lhighgui -lm

all: perf perf_gprof driver

driver: driver.o impl.o mshift.o
	g++ $(LIBS) $^ -o $@
	
perf: perf.o impl.o mshift.o
	g++ $(LIBS) $^ -o $@

perf_gprof: perf.o impl.o mshift.o
	g++ -pg $(LIBS) $^ -o $@

impl.o: impl.cpp header.h

driver.o: driver.cpp header.h
perf.o: driver.cpp header.h

.PHONY: clean
clean:
	rm -f *.o driver perf

test:
	echo tbc
