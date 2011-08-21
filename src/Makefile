
CXXFLAGS= -g
LIBS= -lcv -lhighgui -lm

PROGRAMS= perf perf_gprof driver

all: $(PROGRAMS)

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
	rm -f *.o $(PROGRAMS)

test:
	echo tbc
