CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LIB_SRC = watchdog.c
LIB_OBJ = watchdog.o

.PHONY: all
all: test

# Compile the library object without WATCHDOG_ENABLE
# so it uses the real system malloc/free internally
$(LIB_OBJ): $(LIB_SRC)
	@$(CC) $(CFLAGS) -c $(LIB_SRC) -o $(LIB_OBJ)

# Compile the test target with WATCHDOG_ENABLE
.PHONY: test
test: tests/test.c $(LIB_OBJ)
	@$(CC) $(CFLAGS) -DWATCHDOG_ENABLE tests/test.c $(LIB_OBJ) -o test

.PHONY: clean
clean:
	@rm -rf *.o *.dSYM test *.log
