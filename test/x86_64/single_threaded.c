#include <dlfcn.h>
#include <pthread.h>
#include "../../src/common.h"

static void* read_value(void* arg) {
  ASSERT(*(const char*)arg == 0);
  return arg;
}

int main(int argc, char** argv) {
  ASSERT(argc == 3);
  void* lib = dlopen(argv[1], RTLD_NOW | RTLD_LOCAL);
  ASSERT(lib != NULL);
  const char* value = dlvsym(lib, "__libc_single_threaded_compat", "POLYFILL");
  ASSERT(value != NULL);
  ASSERT(*value == 0);
  pthread_t threads[8];
  for (unsigned i = 0; i < 8; ++i) {
    ASSERT(pthread_create(&threads[i], NULL, read_value, (void*)value) == 0);
  }
  for (unsigned i = 0; i < 8; ++i) {
    void* result = NULL;
    ASSERT(pthread_join(threads[i], &result) == 0);
    ASSERT(result == value);
  }
  ASSERT(*value == 0);
  ASSERT(dlclose(lib) == 0);
  FILE* output = fopen(argv[2], "w");
  ASSERT(output != NULL);
  ASSERT(fputs("OK\n", output) >= 0);
  ASSERT(fclose(output) == 0);
  return 0;
}
