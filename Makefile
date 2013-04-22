CXX=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS= -c
CXXFLAGS= -Wall -W
OUTPUT= sieci
all: sieci

sieci: Pakiet.o icmp.o sockwrap.o main.o
	$(CXX) $(CXXFLAGS) main.o Pakiet.o sockwrap.o icmp.o -o $(OUTPUT)

main.o: main.cpp
	$(CXX) $(CFLAGS) main.cpp 
icmp.o: icmp.cpp
	$(CXX) $(CFLAGS) icmp.cpp icmp.h 
sockwrap.o: sockwrap.cpp
	$(CXX) $(CFLAGS) sockwrap.cpp sockwrap.h 
Pakiet.o: Pakiet.cpp
	$(CXX) $(CFLAGS) Pakiet.cpp Pakiet.h 

clean:
	rm -rf *.o 
distclean:
	rm -rf *.o $(OUTPUT)