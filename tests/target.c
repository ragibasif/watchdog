#define WATCHDOG_ENABLE
#include "../watchdog.h"

int main(int argc, char** argv) {
  w_init(true, false, false);
  if (argc < 2) return 1;

  if (strcmp(argv[1], "leak") == 0) {
    malloc(100);  // intentional leak
  } else if (strcmp(argv[1], "double_free") == 0) {
    void* p = malloc(50);
    free(p);
    free(p);  // intentional error
  } else if (strcmp(argv[1], "overflow") == 0) {
    char* p = malloc(10);
    p[11] = 'X';  // Modify the canary
    free(p);      // Watchdog checks canary during free()
  }

  return 0;
}
