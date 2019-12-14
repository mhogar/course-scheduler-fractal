CC = g++
FLAGS = -Wall -std=c++17
INCLUDE = -Iinclude
LIB_OBJS = obj/schedulerInterface.o obj/scheduler.o obj/dataParser.o
MAIN_OBJS = obj/main.o
TEST_OBJ = obj/testsMain.o
SO = bin/scheduler.so
MAIN_PROGRAM = bin/main
TEST_PROGRAMS = bin/dataParserTests bin/schedulerTests

all: main tests

lib: $(LIB_OBJS)
	$(CC) -shared -o $(SO) $(LIB_OBJS)

main: lib $(MAIN_OBJS)
	$(CC) $(MAIN_OBJS) $(SO) -o $(MAIN_PROGRAM)

test: $(TEST_PROGRAMS)

obj/schedulerInterface.o: src/SchedulerInterface.cpp
	$(CC) $(FLAGS) -fpic $(INCLUDE) -c src/SchedulerInterface.cpp -o obj/schedulerInterface.o

obj/scheduler.o: src/Scheduler.cpp
	$(CC) $(FLAGS) -fpic $(INCLUDE) -c src/Scheduler.cpp -o obj/scheduler.o

obj/dataParser.o: src/DataParser.cpp
	$(CC) $(FLAGS) -fpic $(INCLUDE) -c src/DataParser.cpp -o obj/dataParser.o

obj/main.o: src/main.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c src/main.cpp -o obj/main.o

$(TEST_OBJ): tests/testsMain.cpp
	$(CC) $(FLAGS) $(INCLUDE) -c tests/testsMain.cpp -o $(TEST_OBJ)

bin/dataParserTests: lib $(TEST_OBJ) tests/DataParserTests.cpp
	$(CC) $(FLAGS) $(INCLUDE) $(TEST_OBJ) tests/DataParserTests.cpp $(SO) -o bin/dataParserTests

bin/schedulerTests: lib $(TEST_OBJ) tests/SchedulerTests.cpp
	$(CC) $(FLAGS) $(INCLUDE) $(TEST_OBJ) tests/SchedulerTests.cpp $(SO) -o bin/schedulerTests

clean: clean_lib clean_main clean_test

clean_lib:
	rm $(LIB_OBJS) $(SO)

clean_main:
	rm $(MAIN_OBJS) $(MAIN_PROGRAM)

clean_test:
	rm $(TEST_OBJ) $(TEST_PROGRAMS)
