CFLAGS = -g -masm=intel -no-pie -march=native -O2

all: List.o reading_data.o hash_map.o main.o 	
	g++ main.oList.o reading_data.o hash_map.o -o hashmap

all_optimized: List.o reading_data.o hash_map.o main.o fixed_string_strcmp.o
	g++ $(CFLAGS) main.o fixed_string_strcmp.o List.o reading_data.o hash_map.o -o hashmap

List.o : ./List/MyList.cpp
	g++ $(CFLAGS) -c -o List.o ./List/MyList.cpp

reading_data.o : ./Hash_map/reading_data.cpp
	g++ $(CFLAGS) -c -o reading_data.o ./Hash_map/reading_data.cpp

hash_map.o: ./Hash_map/hash_map.cpp 
	g++ $(CFLAGS) -c -o hash_map.o ./Hash_map/hash_map.cpp

# crc_hash.o: ./Hash_map/crc_hash.cpp
# 	g++ $(CFLAGS) -c -o crc_hash.o ./Hash_map/crc_hash.cpp

fixed_string_strcmp.o: ./Hash_map/fixed_string_strcmp.asm
	nasm -felf64 ./Hash_map/fixed_string_strcmp.asm -o fixed_string_strcmp.o 

main.o : ./Hash_map/main.cpp
	g++ $(CFLAGS) -c -o main.o ./Hash_map/main.cpp

run: all
	./hashmap

run_optimized: all_optimized
	./hashmap

debug: all
	valgrind ./hashmap

debug_optimized: all_optimized
	valgrind ./hashmap

clean: 
	rm *.o

count_time: all
	valgrind --dump-instr=yes --tool=callgrind ./hashmap

count_time_optimize: all_optimized
	valgrind --dump-instr=yes --tool=callgrind ./hashmap
