CFLAGS = -g -masm=intel -no-pie -march=native -O1

all: main.o translator.o file_reader.o elf.o consts.o
	g++ main.o translator.o file_reader.o elf.o consts.o -o translator

main.o : main.cpp
	g++ $(CFLAGS) -c -o main.o main.cpp

translator.o: Translation.cpp
	g++ $(CFLAGS) -c -o translator.o Translation.cpp

file_reader.o: ReadingFromFile.cpp
	g++ $(CFLAGS) -c -o file_reader.o ReadingFromFile.cpp

consts.o: Consts.cpp
	g++ $(CFLAGS) -c -o consts.o Consts.cpp

instructions.o: Instructions_realizations.cpp
	g++ $(CFLAGS) -c -o instructions.o Instructions_realizations.cpp

elf.o: Executable_elf.cpp
	g++ $(CFLAGS) -c -o elf.o Executable_elf.cpp

clean: 
	rm *.o

run: all
	valgrind ./translator
