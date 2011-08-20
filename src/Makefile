
CXXFLAGS= -g
LIBS= -lcv -lhighgui -lm

driver: driver.o impl.o
	g++ $(LIBS) $^ -o $@

impl.o: impl.cpp header.h

driver.o: driver.cpp header.h

clean:
	rm -f *.o driver

test:
	echo tbc
