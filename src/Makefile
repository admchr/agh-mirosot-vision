
CXXFLAGS= -g -Wall -Wextra -O0
LIBS= -lcv -lcxcore -lhighgui -lm
TEST_LIBS= -lboost_unit_test_framework $(LIBS)

PROGRAMS= perf perf_gprof driver interactive test_impl test_api
OBJS= vision.o mshift.o balance.o area.o hsvconverter.o moments.o linearize.o mask.o debug.o util.o
API_HEADERS=amv.h
IMPL_HEADERS=\
    $(API_HEADERS) \
    area.hpp array2d.hpp balance.hpp defs.hpp moments.hpp mshift.hpp hsvconverter.hpp \
    visionstate.hpp linearize.hpp mask.hpp common.hpp debug.hpp util.hpp

all: $(PROGRAMS)

driver: driver.o $(OBJS)
	g++ $^ -o $@ $(LIBS)
	
interactive: interactive.o $(OBJS)
	g++ $^ -o $@ $(LIBS)

perf: perf.o  $(OBJS)
	g++ $^ -o $@ $(LIBS)

perf_gprof: perf.o $(OBJS)
	g++ -pg $^ -o $@ $(LIBS)

test_impl: test_impl.o test_array2d.o test_mshift.o test_linearize.o test_mask.o test_moments.o $(OBJS)
	g++ $^ -o $@ $(TEST_LIBS) 

test_api: test_api.o $(OBJS) $(API_HEADERS)
	gcc test_api.o $(OBJS) -o $@ $(LIBS)

$(OBJS): $(IMPL_HEADERS)
driver.o: common.hpp
perf.o: common.hpp
interactive.o: common.hpp

test_impl.o: test_impl.cpp $(API_HEADERS)
test_array2d.o: test_array2d.cpp array2d.hpp
test_mshift.o: test_mshift.cpp mshift.hpp
test_linearize.o: test_linearize.cpp linearize.hpp
test_mask.o: test_mask.cpp mask.hpp
test_moments.o: test_moments.cpp moments.hpp

.PHONY: clean
clean:
	rm -f *.o $(PROGRAMS)

test: test_impl test_api
	./test_api
	./test_impl
