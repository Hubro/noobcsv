
#include <check.h>
#include "check_noobcsv_write.h"
#include "noobcsv.h"

START_TEST (check_nothing_w)
{
}
END_TEST

TCase* noobcsv_write_tc(void)
{
  TCase *tc_write = tcase_create("Writing");
  tcase_add_test(tc_write, check_nothing_w);

  return tc_write;
}
