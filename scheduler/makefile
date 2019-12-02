CC = g++
FLAGS = -Wall -std=c++17
TEST_OBJS = obj/test.o
LIB_OBJS = obj/scheduler.o
DLL = bin/scheduler.so
PROGRAM = bin/test

program: $(DLL) $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(DLL) -o $(PROGRAM)

$(DLL): $(LIB_OBJS)
	$(CC) -shared -o $(DLL) $(LIB_OBJS)

obj/test.o: test.cpp
	$(CC) $(FLAGS) -c test.cpp -o obj/test.o

obj/scheduler.o: Scheduler.cpp
	$(CC) $(FLAGS) -c Scheduler.cpp -o obj/scheduler.o

clean:
	rm $(TEST_OBJS) $(LIB_OBJS) $(DLL) $(PROGRAM)