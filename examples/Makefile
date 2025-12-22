CC := clang
CFLAGS := -std=c2x -O2
LDFLAGS := -lc -pthread

.PHONY: default
default: all

.PHONY: all
all: main

main: main.o watchdog.o
	@$(CC) $(CFLAGS) $(LDFLAGS) main.o watchdog.o -o main

main.o: main.c
	@$(CC) $(CFLAGS) -c main.c -o main.o

watchdog.o: ../src/watchdog.c
	@$(CC) $(CFLAGS) -c ../src/watchdog.c -o watchdog.o


.PHONY: run
run:
	@make clean
	@make all
	@./main
	@make clean

.PHONY: clean
clean:
	@rm -rf *.o main


