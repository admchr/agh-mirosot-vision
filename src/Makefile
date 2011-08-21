
CXXFLAGS= -g
LIBS= -lcv -lhighgui -lm
TEST_LIBS= -lboost_unit_test_framework $(LIBS)

PROGRAMS= perf perf_gprof driver

all: $(PROGRAMS)

driver: driver.o impl.o mshift.o
	g++ $(LIBS) $^ -o $@
	
perf: perf.o impl.o mshift.o
	g++ $(LIBS) $^ -o $@

perf_gprof: perf.o impl.o mshift.o
	g++ -pg $(LIBS) $^ -o $@

test_impl: test_impl.o mshift.o
	g++ -pg $(TEST_LIBS) $^ -o $@ 

impl.o: impl.cpp header.h

driver.o: driver.cpp header.h
perf.o: driver.cpp header.h
test_impl.o: impl.cpp test_impl.cpp header.h

.PHONY: clean
clean:
	rm -f *.o $(PROGRAMS)



test: test_impl
	./test_impl
