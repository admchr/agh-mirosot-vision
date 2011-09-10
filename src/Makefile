
CXXFLAGS= -g -Wall -Wextra -O2 -pg
LIBS= -lcv -lhighgui -lm
TEST_LIBS= -lboost_unit_test_framework $(LIBS)

PROGRAMS= perf perf_gprof driver test_impl
OBJS= impl.o mshift.o balance.o area.o pixelset.o hsvconverter.o
IMPL_HEADERS= area.hpp array2d.hpp balance.hpp defs.hpp mshift.hpp pixelset.hpp hsvconverter.hpp visionstate.hpp
all: $(PROGRAMS)

driver: driver.o $(OBJS)
	g++ $(LIBS) $^ -o $@
	
perf: perf.o  $(OBJS)
	g++ $(LIBS) $^ -o $@

perf_gprof: perf.o $(OBJS)
	g++ -pg $(LIBS) $^ -o $@

test_impl: test_impl.o test_array2d.o test_mshift.o $(OBJS)
	g++ $(TEST_LIBS)  $^ -o $@

area.o:area.cpp $(IMPL_HEADERS)
impl.o: impl.cpp header.h
balance.o: balance.cpp $(IMPL_HEADERS)
driver.o: driver.cpp header.h
perf.o: driver.cpp header.h
mshift.o: mshift.cpp $(IMPL_HEADERS)
pixelset.o: pixelset.cpp $(IMPL_HEADERS)
hsvconverter.o: hsvconverter.cpp $(IMPL_HEADERS)

test_impl.o: test_impl.cpp header.h
test_array2d.o: test_array2d.cpp array2d.hpp
test_mshift.o: test_mshift.cpp mshift.hpp

.PHONY: clean
clean:
	rm -f *.o $(PROGRAMS)

test: test_impl
	./test_impl
