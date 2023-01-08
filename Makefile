# Makefile
 
PROGRAM = nuigurumin.exe
OBJS = main.o Timer.o

.SUFFIXES: .cpp .o

$(PROGRAM): $(OBJS)
	g++ -std=c++17 -O3 -o $(PROGRAM) $^

.cpp.o:
	g++ -std=c++17 -O3 -c $<

clean:
	rm -f $(PROGRAM) $(OBJS)