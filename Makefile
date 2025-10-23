all: bin utf8hist 2dutf8hist 3dutf8hist 4dutf8hist 

bin: bin.c
	gcc -O2 bin.c -o bin -Wall -Wextra -lm

4dutf8hist: 4dutf8hist.c
	gcc -O2 -Wextra -Wall 4dutf8hist.c -o 4dutf8hist -lncurses -lm

3dutf8hist: 3dutf8hist.c
	gcc -O2 -Wextra -Wall 3dutf8hist.c -o 3dutf8hist -lncurses -lm

2dutf8hist: 2dutf8hist.c
	gcc -O2 -Wextra -Wall 2dutf8hist.c -o 2dutf8hist -lm

utf8hist: utf8hist.cpp
	g++ -Wextra -Wall utf8hist.cpp -o utf8hist
