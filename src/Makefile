
CXXFLAGS= -g -Wall -Wextra
LIBS= -lcv -lhighgui -lm
TEST_LIBS= -lboost_unit_test_framework $(LIBS)

PROGRAMS= perf perf_gprof driver
OBJS= impl.o mshift.o balance.o
all: $(PROGRAMS)

driver: driver.o $(OBJS)
	g++ $(LIBS) $^ -o $@
	
perf: perf.o  $(OBJS)
	g++ $(LIBS) $^ -o $@

perf_gprof: perf.o $(OBJS)
	g++ -pg $(LIBS) $^ -o $@

test_impl: test_impl.o $(OBJS)
	g++ -pg $(TEST_LIBS) $^ -o $@ 

impl.o: impl.cpp header.h balance.h
balance.o: balance.cpp balance.h
driver.o: driver.cpp header.h
perf.o: driver.cpp header.h
test_impl.o: impl.cpp test_impl.cpp header.h

.PHONY: clean
clean:
	rm -f *.o $(PROGRAMS)



test: test_impl
	./test_impl
