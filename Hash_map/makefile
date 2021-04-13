all: list data hash_map main
	g++ main.o List.o reading_data.o hash_map.o -o hashmap

list : ./List/MyList.cpp
	g++ -c -o List.o ./List/MyList.cpp

data : ./Hash_map/reading_data.cpp
	g++ -c -o reading_data.o ./Hash_map/reading_data.cpp

hash_map: ./Hash_map/hash_map.cpp
	g++ -c -o hash_map.o ./Hash_map/hash_map.cpp

main : ./Hash_map/main.cpp
	g++ -c -o main.o ./Hash_map/main.cpp

run: all
	./hashmap

debug: all
	valgrind ./hashmap

count_time: all
	valgrind --tool=callgrind ./hashmap
