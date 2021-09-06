CXX	= g++ -std=c++17
INCLUDES	=
CXXFLAGS	= -g

LDFLAGS	= -pthread
OPTFLAGS	= -O3 -finline-functions

OBJECTS	= utilities/point.o utilities/utilities.o

TARGETS	= sequential threads_barrier threads_map threads_pool ff_parallel_reduce ff_farm

.PHONY: all clean cleanall
.SUFFIXES: .cpp

all	: $(TARGETS)

sequential: $(OBJECTS) sequential.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(OBJECTS) sequential.o

threads_barrier: $(OBJECTS) threads_barrier.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(OBJECTS) threads_barrier.o $(LDFLAGS)

threads_map: $(OBJECTS) threads_map.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(OBJECTS) threads_map.o $(LDFLAGS)

threads_pool: $(OBJECTS) threads_pool.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(OBJECTS) threads_pool.o $(LDFLAGS)

ff_parallel_reduce: $(OBJECTS) ff_parallel_reduce.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(OBJECTS) ff_parallel_reduce.o $(LDFLAGS)

ff_farm: $(OBJECTS) ff_farm.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ $(OBJECTS) ff_farm.o $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPTFLAGS) -o $@ -c $< $(LDFLAGS)

clean:
	rm -f $(TARGETS)

cleanall:	clean
	rm -f utilities/*.o
	rm -f *.txt *~
	rm -f *.exe *~
	rm -f *.o *~