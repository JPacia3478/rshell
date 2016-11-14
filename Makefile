all: rshell

rshell: main.cpp
	g++ -Wall -Werror -ansi -pedantic main.cpp -o rshell