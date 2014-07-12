
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

#define ASSERT_INT_EQ(i1, i2) \
  ck_assert_msg( \
    (i1) == (i2), \
    "Assertion '" #i1 " == " #i2 "' failed: " \
      #i1 " = %d, " #i2 " = %d", \
    (i1), (i2) \
  )

#define ASSERT_CHAR_EQ(c1, c2) \
  ck_assert_msg( \
    (c1) == (c2), \
    "Assertion '" #c1 " == " #c2 "' failed: " \
      #c1 " = '%c', " #c2 " = '%c'", \
    (c1), (c2) \
  )

#define ASSERT_STR_EQ(s1, s2) \
  ck_assert_msg( \
    strcmp((s1), (s2)) == 0, \
    "Assertion '%s == %s' failed: %s = \"%s\", %s = \"%s\"", \
    #s1, #s2, #s1, s1, #s2, s2 \
  )

#define ASSERT_CHAR_ARRAY_EQ(a1, a2, size) \
  assert_char_array_eq(a1, a2, #a1, #a2, size)

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

static int assert_char_array_eq(char *a1, char *a2,
                                char *as1, char *as2,
                                ssize_t size)
{
  ck_assert_msg(
    memcmp(a1, a2, size) == 0,
    "Assertion \"%s\" == \"%s\" failed: %s = \"%.*s\", %s = \"%.*s\"",
    as1, as2, as1, size, a1, as2, size, a2
  );
}

#endif
