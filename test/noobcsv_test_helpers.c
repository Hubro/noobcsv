
#ifndef NOOBCSV_TEST_HELPERS_C
#define NOOBCSV_TEST_HELPERS_C

#define SET_UP_TEST_HANDLE(file_name, mode) \
  FILE *file = open_test_file(file_name, mode); \
  ck_assert_msg(file != NULL, "Failed to open test file"); \
  struct NoobCSVOptions opts = noobcsv_create_opts(); \
  NoobCSVHandle *handle = noobcsv_create_handle(file, &opts)

#define FREE_TEST_HANDLE \
  noobcsv_free_handle(handle); \
  fclose(file)

#include <stdlib.h>   /* malloc */
#include <unistd.h>   /* readlink */
#include <libgen.h>   /* dirname */

static FILE *open_test_file(char *name, char *mode)
{
  size_t bufsize = 512;
  char path_buffer[bufsize];

  /* Fetches the path to the current executable. Linux only! */
  readlink("/proc/self/exe", path_buffer, bufsize);

  char* dir = dirname(path_buffer);

  strcpy(path_buffer, dir);
  strcat(path_buffer, "/../common/");
  strcat(path_buffer, name);

  FILE *file = fopen(path_buffer, mode);

  return file;
}

#endif
