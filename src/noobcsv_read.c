
#include <stdlib.h>   /* malloc */

#include "noobcsv.h"
#include "noobcsv_private.h"

static int ready_buffer(NoobCSVHandle *handle);
static void fill_buffer(NoobCSVHandle *handle);
static noobcsv_ct consume_char(NoobCSVHandle *handle, char *out);
static noobcsv_ct consume_char_nopeek(NoobCSVHandle *handle, noobcsv_ct peek);
static noobcsv_ct peek_char(NoobCSVHandle *handle, char *out);
static noobcsv_tdt on_text_delimiter(NoobCSVHandle *handle);
static int is_record_delimiter(char c, NoobCSVHandle *handle);

int noobcsv_next_field(NoobCSVHandle *handle)
{
  noobcsv_ct ct;

  if (!ready_buffer(handle))
    return 0;

  consume_char(handle, NULL);

  while (1) {
    ct = consume_char(handle, NULL);

    if (ct == NOOBCSV_CT_EOF)
      return 0;
    else if (ct == NOOBCSV_CT_FDELIM)
      return 1;
    else if (ct == NOOBCSV_CT_RDELIM && !handle->in_text)
      return 1;
  }
}

int noobcsv_next_record(NoobCSVHandle *handle)
{
  noobcsv_ct ct;

  if (!ready_buffer(handle))
    return 0;

  consume_char(handle, NULL);

  while (1) {
    ct = consume_char(handle, NULL);

    if (ct == NOOBCSV_CT_EOF)
      return 0;
    else if (ct == NOOBCSV_CT_RDELIM && !handle->in_text)
      return 1;
  }
}

/* Fills the buffer if it hasn't been filled yet. Returns 0 if there is nothing
 * more to read. */
static int ready_buffer(NoobCSVHandle *handle)
{
  /* First read */
  if (handle->readbufcrs == -1) {
    fseek(handle->file, 0, SEEK_SET);

    fill_buffer(handle);
  }

  return 1;
}

/* Fills the buffer with new data from the file, starting from two positions
 * before the current cursor position. Sets the read cursor to 2. */
static void fill_buffer(NoobCSVHandle *handle)
{
  int bytes_read;

  /* Start of the file? */
  if (ftell(handle->file) == 0) {
    bytes_read = fread(
      handle->readbuf, sizeof(char), handle->bufsize, handle->file
    );

    handle->readbufcrs = 0;
  }
  else {
    int remainder = (handle->bufsize - 1) - handle->readbufcrs;

    /* Jump back to 2 characters before the read cursor */
    fseek(handle->file, (-3) - remainder, SEEK_CUR);

    bytes_read = fread(
      handle->readbuf, sizeof(char), handle->bufsize, handle->file
    );

    handle->readbufcrs = 2;
  }

  if (bytes_read < handle->bufsize)
    handle->readbuf[bytes_read] = '\0';
}

/* Consumes one character (or two, in a couple of cases.) If the character
 * consumed is the final line break before EOF, NOOBCSV_CT_EOF is returned
 * instead of NOOBCSV_CT_RDELIM. */
static noobcsv_ct consume_char(NoobCSVHandle *handle, char *out)
{
  noobcsv_ct ct = peek_char(handle, out);

  consume_char_nopeek(handle, ct);

  return ct;
}

/* Consumes a character, but skips the "peek" step. This saves work if the
 * caller has already peeked. */
static noobcsv_ct consume_char_nopeek(NoobCSVHandle *handle, noobcsv_ct peek)
{
  noobcsv_ct ct = peek;

  /* Nothing more to consume */
  if (ct == NOOBCSV_CT_EOF)
    return ct;

  /* Should an additional character be consumed? */
  noobcsv_tdt t = on_text_delimiter(handle);

  if (t == NOOBCSV_TDT_ESCAPED) {
    handle->readbufcrs++;
  }
  else {
    if (handle->readbuf[handle->readbufcrs] == '\r' &&
        handle->readbuf[handle->readbufcrs + 1] == '\n')
      handle->readbufcrs++;
  }

  /* Now between text delimiters? */
  if (ct == NOOBCSV_CT_TDELIM_OPEN)
    handle->in_text = 1;
  else if (ct == NOOBCSV_CT_TDELIM_CLOSE)
    handle->in_text = 0;

  /* Consume the character */
  handle->readbufcrs++;

  /* Only 1 or less chars left in the buffer? Time for a refill */
  if (handle->readbufcrs >= handle->bufsize - 2)
    fill_buffer(handle);

  return ct;
}

static noobcsv_ct peek_char(NoobCSVHandle *handle, char *out)
{
  FILE *file = handle->file;
  char *buffer = handle->readbuf;
  ssize_t bufsize = handle->bufsize;
  noobcsv_ct retval = NOOBCSV_CT_TEXT;

  /* Are we on the last char? */
  if (buffer[handle->readbufcrs] == '\0')
    return NOOBCSV_CT_EOF;

  char curchar = buffer[handle->readbufcrs];
  char nextchar = buffer[handle->readbufcrs + 1];

  if (out != NULL)
    *out = curchar;

  /* Current character is a field delimiter? */
  if (curchar == handle->opts->field_delimiter && !handle->in_text) {
    retval = NOOBCSV_CT_FDELIM;
    goto done;
  }

  /* Current character is a text delimiter? */
  noobcsv_tdt tdt = on_text_delimiter(handle);

  if (tdt) {
    switch(tdt) {
      case NOOBCSV_TDT_OPENING:
        retval = NOOBCSV_CT_TDELIM_OPEN;
        goto done;
      case NOOBCSV_TDT_CLOSING:
        retval = NOOBCSV_CT_TDELIM_CLOSE;
        goto done;
      case NOOBCSV_TDT_ESCAPED:
        retval = NOOBCSV_CT_TEXT;
        goto done;
    }
  }

  /* Are we between text delimiters? */
  if (handle->in_text) {
    retval = NOOBCSV_CT_TEXT;
    goto done;
  }

  /* Current character is a record delimiter? */
  if (handle->opts->auto_line_endings) {
    if (curchar == '\n' || curchar == '\r')
      retval = NOOBCSV_CT_RDELIM;
  }
  else {
    if (curchar == handle->opts->record_delimiter)
      retval = NOOBCSV_CT_RDELIM;
  }

done:

  /* Was this the final line break of the file? */
  if (retval == NOOBCSV_CT_RDELIM) {
    if (buffer[handle->readbufcrs + 1] == '\0')
      retval = NOOBCSV_CT_EOF;
    if (buffer[handle->readbufcrs + 1] == '\r' &&
        buffer[handle->readbufcrs + 2] == '\0')
      retval = NOOBCSV_CT_EOF;
  }

  return retval;
}

/* A text delimiter is only escaped if the NEXT character is a text delimiter,
 * "consume_char" will make sure of that. */
static noobcsv_tdt on_text_delimiter(NoobCSVHandle *handle)
{
  char tdelim = handle->opts->text_delimiter;
  char fdelim = handle->opts->field_delimiter;
  char *buffer = handle->readbuf;
  int crs = handle->readbufcrs;

  char c = buffer[crs];
  char nc = buffer[crs + 1];
  char pc = buffer[crs - 1];

  if (c == tdelim) {
    /* Text delimiter as the first char of the file? */
    if (crs == 0)
      return NOOBCSV_TDT_OPENING;

    /* Is this an opening text delimiter? Opening text delimiters can't be
     * escaped */
    if (!handle->in_text)
      if (pc == fdelim || is_record_delimiter(pc, handle))
        return NOOBCSV_TDT_OPENING;

    /* Escaped? */
    if (nc == tdelim)
      return NOOBCSV_TDT_ESCAPED;

    /* Is this a closing text delimiter? */
    if (handle->in_text)
      if (nc == fdelim || is_record_delimiter(nc, handle))
        return NOOBCSV_TDT_CLOSING;
  }

  return 0;
}

/* Returns true if the input character is a line break */
static int is_record_delimiter(char c, NoobCSVHandle *handle)
{
  if (handle->opts->auto_line_endings) {
    if (c == '\r' || c == '\n')
      return 1;
    else
      return 0;
  }
  else {
    return c == handle->opts->record_delimiter;
  }
}
