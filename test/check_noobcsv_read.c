
#include <stdio.h>
#include <check.h>
#include "noobcsv.h"
#include "noobcsv_private.h"
#include "noobcsv_test_helpers.c"

START_TEST (check_next_field)
{
  SET_UP_TEST_HANDLE ("simple.csv", "r");

  ck_assert_int_eq(handle->readbufcrs, -1);

  /* Expected indexes */
  int ei[] = {2, 4, 6, 8, 10, 12, 14, 16};
  int count = sizeof(ei) / sizeof(ei[0]);
  int i, result;

  for (i = 0; i < count; i++) {
    result = noobcsv_next_field(handle);
    ck_assert_int_eq(result, 1);
    ck_assert_int_eq(ei[i], handle->readbufcrs);
  }

  /* There are no more fields, a seek should now fail */
  result = noobcsv_next_field(handle);
  ck_assert_int_eq(result, 0);

  /* The cursor should rest on the last byte of the file */
  ck_assert_int_eq(handle->readbufcrs, 17);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST (check_next_field_quotes)
{
  SET_UP_TEST_HANDLE ("quoted.csv", "r");

  ck_assert_int_eq(-1, handle->readbufcrs);

  /* Expected indexes */
  int ei[] = {6, 12, 18, 21, 28, 33, 40, 49, 53, 71, 74};
  int count = sizeof(ei) / sizeof(ei[0]);
  int i, result;

  for (i = 0; i < count; i++) {
    result = noobcsv_next_field(handle);
    ck_assert_int_eq(result, 1);
    ck_assert_int_eq(ei[i], handle->readbufcrs);
  }

  /* There are no more fields, a seek should now fail */
  result = noobcsv_next_field(handle);
  ck_assert_int_eq(result, 0);

  /* The cursor should rest on the last byte of the file */
  ck_assert_int_eq(handle->readbufcrs, 78);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST (check_next_field_mini)
{
  SET_UP_TEST_HANDLE ("minimum.csv", "r");

  ck_assert_int_eq(-1, handle->readbufcrs);

  int result = noobcsv_next_field(handle);
  ck_assert_int_eq(result, 0);

  ck_assert_int_eq(handle->readbufcrs, 1);

  FREE_TEST_HANDLE;
}
END_TEST

TCase* noobcsv_read_tc(void)
{
  TCase *tc_write = tcase_create("Reading");
  tcase_add_test(tc_write, check_next_field);
  tcase_add_test(tc_write, check_next_field_quotes);
  tcase_add_test(tc_write, check_next_field_mini);

  return tc_write;
}
