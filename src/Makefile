
CXXFLAGS= -g -Wall -Wextra -O2
LIBS= -lcv -lhighgui -lm
TEST_LIBS= -lboost_unit_test_framework $(LIBS)

PROGRAMS= perf perf_gprof driver test_impl
OBJS= impl.o mshift.o balance.o area.o
all: $(PROGRAMS)

driver: driver.o $(OBJS)
	g++ $(LIBS) $^ -o $@
	
perf: perf.o  $(OBJS)
	g++ $(LIBS) $^ -o $@

perf_gprof: perf.o $(OBJS)
	g++ -pg $(LIBS) $^ -o $@

test_impl: test_impl.o test_array2d.o $(OBJS)
	g++ $(TEST_LIBS)  $^ -o $@

area.o:area.cpp area.hpp mshift.hpp pixelset.hpp
impl.o: impl.cpp header.h balance.hpp area.hpp pixelset.hpp
balance.o: balance.cpp balance.hpp
driver.o: driver.cpp header.h
perf.o: driver.cpp header.h
mshift.o: mshift.cpp mshift.hpp

test_impl.o: test_impl.cpp header.h
test_array2d.o: test_array2d.cpp array2d.hpp

.PHONY: clean
clean:
	rm -f *.o $(PROGRAMS)

test: test_impl
	./test_impl
