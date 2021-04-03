all:
	g++ -c -g -O1 -march=native main.cpp
	g++ main.o -o cat_on_table -lsfml-graphics -lsfml-window -lsfml-system

run: all
	./cat_on_table

debug: all
	valgrind ./cat_on_table
