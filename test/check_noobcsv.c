
#include <stdlib.h>
#include <check.h>
#include "noobcsv.h"

START_TEST (check_options_struct)
{
  NoobCSVOptions opts = {
    .first_line = 12;
    .last_line = 34;
    .line_count = -1;

    .field_delimiter = 'f';
    .text_delimiter = 't';
    .line_endings = 'l';

    .auto_line_endings = 23;
  };

  /* Nothing exploded? Great! */
}
END_TEST

START_TEST (check_init_opts)
{
  NoobCSVOptions opts;

  noobcsv_init_opts(&opts);

  ck_assert_int_eq(-1, opts.first_line);
  ck_assert_chr_eq(',', opts.field_delimiter);
  ck_assert_int_eq(1, opts.auto_line_endings);
}
END_TEST

Suite *
noobcsv_suite(void)
{
  Suite* s = suite_create("noobcsv");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, check_options_struct);
  tcase_add_test(tc_core, check_init_opts);

  suite_add_tcase(s, tc_core);

  return s;
}

void main() {
  int number_failed = 0;

  Suite* s = noobcsv_suite();

  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);

  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
