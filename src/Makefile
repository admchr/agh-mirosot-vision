
CXXFLAGS= -g -Wall -Wextra -O2
LIBS= -lcv -lcxcore -lhighgui -lm
TEST_LIBS= -lboost_unit_test_framework $(LIBS)

PROGRAMS= perf perf_gprof driver test_impl
OBJS= impl.o mshift.o balance.o area.o pixelset.o hsvconverter.o moments.o linearize.o mask.o
IMPL_HEADERS=\
    area.hpp array2d.hpp balance.hpp defs.hpp moments.hpp mshift.hpp pixelset.hpp hsvconverter.hpp \
    visionstate.hpp linearize.hpp mask.hpp common.hpp

all: $(PROGRAMS)

driver: driver.o $(OBJS)
	g++ $^ -o $@ $(LIBS)
	
perf: perf.o  $(OBJS)
	g++ $^ -o $@ $(LIBS)

perf_gprof: perf.o $(OBJS)
	g++ -pg $^ -o $@ $(LIBS)

test_impl: test_impl.o test_array2d.o test_mshift.o test_linearize.o test_mask.o $(OBJS)
	g++ $^ -o $@ $(TEST_LIBS) 

area.o:area.cpp $(IMPL_HEADERS)
impl.o: impl.cpp header.h $(IMPL_HEADERS)
balance.o: balance.cpp $(IMPL_HEADERS)
driver.o: driver.cpp header.h common.hpp
perf.o: driver.cpp header.h common.hpp
mshift.o: mshift.cpp $(IMPL_HEADERS)
pixelset.o: pixelset.cpp $(IMPL_HEADERS)
hsvconverter.o: hsvconverter.cpp $(IMPL_HEADERS)
linearize.o: linearize.cpp $(IMPL_HEADERS)
mask.o: mask.cpp $(IMPL_HEADERS)

test_impl.o: test_impl.cpp header.h
test_array2d.o: test_array2d.cpp array2d.hpp
test_mshift.o: test_mshift.cpp mshift.hpp
test_linearize.o: test_linearize.cpp linearize.hpp
test_mask.o: test_mask.cpp mask.hpp

.PHONY: clean
clean:
	rm -f *.o $(PROGRAMS)

test: test_impl
	./test_impl
