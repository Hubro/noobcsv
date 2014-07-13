
#include <stdio.h>
#include <check.h>
#include "check_noobcsv_read.h"
#include "noobcsv_test_helpers.c"

#include "noobcsv_read.c"

/************************
 *  Exported functions  *
 ************************/

START_TEST(check_next_field)
{
  SET_UP_TEST_HANDLE ("simple.csv", "r");

  ASSERT_INT_EQ(handle->readbufcrs, -1);

  /* Expected indexes */
  int ei[] = {2, 4, 6, 8, 10, 12, 14, 16};
  int count = sizeof(ei) / sizeof(ei[0]);
  int i, result;

  for (i = 0; i < count; i++) {
    result = noobcsv_next_field(handle);
    ASSERT_INT_EQ(result, 1);
    ASSERT_INT_EQ(handle->readbufcrs, ei[i]);
  }

  /* There are no more fields, a seek should now fail */
  result = noobcsv_next_field(handle);
  ASSERT_INT_EQ(result, 0);

  /* The cursor should rest on the null-byte after the last byte of the file */
  ASSERT_INT_EQ(handle->readbufcrs, 18);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST(check_next_field_quotes)
{
  SET_UP_TEST_HANDLE ("quoted.csv", "r");

  ASSERT_INT_EQ(-1, handle->readbufcrs);

  /* Expected indexes */
  int ei[] = {6, 12, 18, 21, 28, 33, 40, 49, 53, 75, 78};
  int count = sizeof(ei) / sizeof(ei[0]);
  int i, result;

  for (i = 0; i < count; i++) {
    result = noobcsv_next_field(handle);
    ASSERT_INT_EQ_MSG(
      result, 1,
      "result was %d for loop index %d (expected index %d)", result, i, ei[i]
    );
    ASSERT_INT_EQ(handle->readbufcrs, ei[i]);
  }

  /* There are no more fields, a seek should now fail */
  result = noobcsv_next_field(handle);
  ASSERT_INT_EQ(result, 0);

  /* The cursor should rest on the null-byte after the last byte of the file */
  ASSERT_INT_EQ(handle->readbufcrs, 81);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST(check_next_field_mini)
{
  SET_UP_TEST_HANDLE ("minimum.csv", "r");

  ASSERT_INT_EQ(-1, handle->readbufcrs);

  int result = noobcsv_next_field(handle);
  ASSERT_INT_EQ(result, 0);

  ASSERT_INT_EQ(handle->readbufcrs, 2);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST(check_next_record)
{
  SET_UP_TEST_HANDLE("simple.csv", "r");

  ASSERT_INT_EQ(handle->readbufcrs, -1);

  /* Expected indexes */
  int ei[] = {6, 12};
  int count = sizeof(ei) / sizeof(ei[0]);
  int i, result;

  for (i = 0; i < count; i++) {
    result = noobcsv_next_record(handle);
    ASSERT_INT_EQ(result, 1);
    ASSERT_INT_EQ(handle->readbufcrs, ei[i]);
  }

  /* There are no more records, a seek should now fail */
  result = noobcsv_next_record(handle);
  ASSERT_INT_EQ(result, 0);

  /* The cursor should rest on the null-byte after the last byte of the file */
  ASSERT_INT_EQ(handle->readbufcrs, 18);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST(check_next_record_quotes)
{
  SET_UP_TEST_HANDLE ("quoted.csv", "r");

  ASSERT_INT_EQ(-1, handle->readbufcrs);

  /* Expected indexes */
  int ei[] = {18, 33, 53};
  int count = sizeof(ei) / sizeof(ei[0]);
  int i, result;

  for (i = 0; i < count; i++) {
    result = noobcsv_next_record(handle);
    ASSERT_INT_EQ(result, 1);
    ASSERT_INT_EQ(ei[i], handle->readbufcrs);
  }

  /* There are no more fields, a seek should now fail */
  result = noobcsv_next_record(handle);
  ASSERT_INT_EQ(result, 0);

  /* The cursor should rest on the null-byte after the last byte of the file */
  ASSERT_INT_EQ(handle->readbufcrs, 81);

  FREE_TEST_HANDLE;
}
END_TEST

/**********************
 *  Static functions  *
 **********************/

START_TEST(check_fill_buffer)
{
  char *expected_buf;

  SET_UP_TEST_HANDLE("quoted.csv", "r");

  /* Sets buffer size to 8 bytes */
  int result = noobcsv_set_bufsize(handle, 8);
  ASSERT_INT_EQ(result, 1);

  fill_buffer(handle);

  ASSERT_INT_EQ(handle->readbufcrs, 0);

  expected_buf = "\"foo\",\"b";
  ASSERT_CHAR_ARRAY_EQ(handle->readbuf, expected_buf, handle->bufsize);

  handle->readbufcrs = 5;
  fill_buffer(handle);

  expected_buf = "o\",\"bar\"";
  ASSERT_CHAR_ARRAY_EQ(handle->readbuf, expected_buf, handle->bufsize);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST(check_fill_buffer_eof)
{
  char *expected_buf;

  SET_UP_TEST_HANDLE("quoted.csv", "r");

  /* Sets buffer size to 8 bytes */
  int result = noobcsv_set_bufsize(handle, 8);
  ASSERT_INT_EQ(result, 1);

  /* This simulates an empty buffer 4 bytes from EOF */
  fseek(handle->file, -3, SEEK_END);
  handle->readbufcrs = handle->bufsize - 1;

  fill_buffer(handle);

  /* Buffer should contain the last 7 bytes of the file */
  expected_buf = "\"\",\"\"\n";
  ASSERT_CHAR_ARRAY_EQ(handle->readbuf, expected_buf, 7);

  FREE_TEST_HANDLE;
}
END_TEST

START_TEST(check_fill_buffer_whole_file)
{
  char *expected_buf;

  SET_UP_TEST_HANDLE("quoted.csv", "r");

  fill_buffer(handle);

  expected_buf = "\"foo\",\"bar\",\"baz\"\n"
                 "no,quotes,here\n"
                 "partly,\"quoted\",row\n"
                 "\"\"\"tricky\"\", \"\"one\"\"\",\"\",\"\"\n";

  ASSERT_STR_EQ(handle->readbuf, expected_buf);

  FREE_TEST_HANDLE;
}
END_TEST

TCase* noobcsv_read_tc(void)
{
  TCase *tc_write = tcase_create("Reading");

  tcase_add_test(tc_write, check_next_field);
  tcase_add_test(tc_write, check_next_field_quotes);
  tcase_add_test(tc_write, check_next_field_mini);

  tcase_add_test(tc_write, check_next_record);
  tcase_add_test(tc_write, check_next_record_quotes);

  tcase_add_test(tc_write, check_fill_buffer);
  tcase_add_test(tc_write, check_fill_buffer_eof);
  tcase_add_test(tc_write, check_fill_buffer_whole_file);

  return tc_write;
}
