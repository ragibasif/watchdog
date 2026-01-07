CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LIB_SRC = watchdog.c
LIB_OBJ = watchdog.o

.PHONY: all
all: target_bin

# Compile the library object without WATCHDOG_ENABLE
# so it uses the real system malloc/free internally
$(LIB_OBJ): $(LIB_SRC)
	@$(CC) $(CFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)

# Compile the test target with WATCHDOG_ENABLE
target_bin: tests/target.c $(LIB_OBJ)
	@$(CC) $(CFLAGS) -DWATCHDOG_ENABLE tests/target.c $(LIB_OBJ) -o target

.PHONY: clean
clean:
	@rm -rf *.o *.dSYM target *.log
