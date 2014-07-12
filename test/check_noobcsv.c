
#include <stdlib.h>
#include <check.h>
#include "noobcsv.h"

#include "check_noobcsv_write.h"
#include "check_noobcsv_read.h"

START_TEST (check_options_struct)
{
  struct NoobCSVOptions opts = {
    .field_delimiter = 'f',
    .text_delimiter = 't',
    .line_endings = 'l',

    .auto_line_endings = 23
  };

  /* Nothing exploded? Great! */
}
END_TEST

START_TEST (check_create_opts)
{
  struct NoobCSVOptions opts = noobcsv_create_opts();

  ck_assert_int_eq(',', opts.field_delimiter);
  ck_assert_int_eq('"', opts.text_delimiter);
  ck_assert_int_eq('\n', opts.line_endings);
  ck_assert_int_eq(1, opts.auto_line_endings);
}
END_TEST

TCase *noobcsv_core_tc(void)
{
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, check_options_struct);
  tcase_add_test(tc_core, check_create_opts);

  return tc_core;
}

Suite *noobcsv_suite(void)
{
  Suite* s = suite_create("noobcsv");

  suite_add_tcase(s, noobcsv_core_tc());
  suite_add_tcase(s, noobcsv_write_tc());
  suite_add_tcase(s, noobcsv_read_tc());

  return s;
}

int main()
{
  int number_failed = 0;

  Suite* s = noobcsv_suite();

  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);

  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
