
#include <stdlib.h>   /* malloc */

#include "noobcsv.h"
#include "noobcsv_private.h"

static void fill_buffer(NoobCSVHandle *handle);
static noobcsv_ct consume_char(NoobCSVHandle *handle, char *out);
static int on_text_delimiter(NoobCSVHandle *handle);
static int is_line_break(char c, NoobCSVHandle *handle);

int noobcsv_next_field(NoobCSVHandle *handle)
{
  FILE *file = handle->file;
  char *buffer = handle->readbuf;
  ssize_t bufsize = handle->bufsize;
  noobcsv_ct ct;

  /* First read */
  if (handle->readbufcrs == -1) {
    fseek(file, 0, SEEK_SET);

    fill_buffer(handle);
  }

  /* Nothing more to read */
  if (buffer[handle->readbufcrs] == '\0')
    return 0;

  consume_char(handle, NULL);

  while (1) {
    ct = consume_char(handle, NULL);

    if (ct == NOOBCSV_EOF)
      return 0;
    else if (ct == NOOBCSV_FDELIM || ct == NOOBCSV_LINE_BREAK)
      return 1;
  }
}

int noobcsv_next_record(NoobCSVHandle *handle)
{

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

/* Consumes one character (or possibly two, if auto_line_endings is 1).
 * If the character consumed is just the final line break before EOF,
 * NOOBCSV_EOF is returned instead of NOOBCSV_LINE_BREAK. */
static noobcsv_ct consume_char(NoobCSVHandle *handle, char *out)
{
  FILE *file = handle->file;
  char *buffer = handle->readbuf;
  ssize_t bufsize = handle->bufsize;
  noobcsv_ct retval = NOOBCSV_TEXT;

  /* Are we on the last char? */
  if (buffer[handle->readbufcrs] == '\0')
    return NOOBCSV_EOF;

  char curchar = buffer[handle->readbufcrs];
  char nextchar = buffer[handle->readbufcrs + 1];

  if (out != NULL)
    *out = curchar;

  /* Current character is a field delimiter? */
  if (curchar == handle->opts->field_delimiter && !handle->in_text)
    retval = NOOBCSV_FDELIM;

  /* Current character is a text delimiter? */
  if (on_text_delimiter(handle)) {
    if (!handle->in_text) {
      handle->in_text = 1;
      retval = NOOBCSV_TDELIM_OPEN;
    }
    else {
      handle->in_text = 0;
      retval = NOOBCSV_TDELIM_CLOSE;
    }
  }

  /* Current character is a line break? */
  if (handle->opts->auto_line_endings) {
    if (curchar == '\n' || curchar == '\r')
      retval = NOOBCSV_LINE_BREAK;

    /* If this is the '\r' in "\r\n", the cursor is moved ahead one additional
     * time */
    if (curchar == '\r' && nextchar == '\n')
      handle->readbufcrs++;
  }

  handle->readbufcrs++;

  /* Was this the final line break of the file? */
  if (retval == NOOBCSV_LINE_BREAK && buffer[handle->readbufcrs] == '\0')
    retval = NOOBCSV_EOF;

  /* Only 1 more char in the buffer? Time for a refill */
  if (handle->readbufcrs == bufsize - 2)
    fill_buffer(handle);

  return retval;
}

static int on_text_delimiter(NoobCSVHandle *handle)
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
      return 1;

    /* Is this an opening text delimiter? */
    if (!handle->in_text) {
      if (pc == fdelim || is_line_break(pc, handle))
        return 1;
    }

    /* Is this a closing text delimiter? */
    if (handle->in_text) {
      /* Escaped? */
      if (pc == tdelim)
        return 0;

      if (nc == fdelim || is_line_break(nc, handle))
        return 1;
    }
  }

  return 0;
}

/* Returns true if the input character is a line break */
static int is_line_break(char c, NoobCSVHandle *handle)
{
  if (handle->opts->auto_line_endings) {
    if (c == '\r' || c == '\n')
      return 1;
    else
      return 0;
  }
  else {
    return c == handle->opts->line_endings;
  }
}
