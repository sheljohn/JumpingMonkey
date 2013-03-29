CC=g++
CFLAGS=-W -pedantic -std=c++0x

jumping_monkey: main.cpp benchmark.cpp chuck.cpp forest.cpp data_structures.cpp
	$(CC) -o $@ $(CFLAGS) $^

test_benchmark: test_benchmark.cpp benchmark.cpp chuck.cpp forest.cpp data_structures.cpp
	$(CC) -o $@ $(CFLAGS) $^

#test_forest: forest.cpp data_structures.cpp test_forest.cpp
#	$(CC) -o $@ $(CFLAGS) $^

#test_data_structure: data_structures.cpp test_data_structure.cpp
#	$(CC) -o $@ $(CFLAGS) $^
