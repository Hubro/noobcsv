
#include <stdlib.h>   /* malloc */

#include "noobcsv.h"
#include "noobcsv_private.h"

/* Fills the read buffer with the next "bufsize" bytes from the file */
static void fill_buffer(NoobCSVHandle *handle);
static int on_text_delimiter(NoobCSVHandle *handle);
static int on_start_of_field(NoobCSVHandle *handle);
static int is_line_break(char c, NoobCSVHandle *handle);

int noobcsv_next_field(NoobCSVHandle *handle)
{
  FILE *file = handle->file;
  char *buffer = handle->readbuf;
  ssize_t bufsize = handle->bufsize;

  /* First read */
  if (handle->readbufcrs == -1) {
    fseek(file, 0, SEEK_SET);

    fill_buffer(handle);
  }

  /* Nothing more to read */
  if (buffer[handle->readbufcrs] == '\0')
    return 0;

  while (1) {
    /* Are we on the last char? */
    if (buffer[handle->readbufcrs + 1] == '\0')
      return 0;

    handle->readbufcrs++;

    /* Only 1 more char in the buffer? Time for a refill */
    if (handle->readbufcrs == bufsize - 2) {
      fill_buffer(handle);
    }

    char curchar = buffer[handle->readbufcrs];
    char nextchar = buffer[handle->readbufcrs + 1];

    if (on_text_delimiter(handle)) {
      /* An opening text delimiter is the start of a field */
      if (!handle->in_text) {
        handle->in_text = 1;
        break;
      }
      else {
        handle->in_text = 0;
        continue;
      }
    }

    if (on_start_of_field(handle))
      break;
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

/* A character is the start of a field if it is directly after a field
 * delimiter or if it is the first character after a line break, even if the
 * character is a text delimiter */
static int on_start_of_field(NoobCSVHandle *handle)
{
  char *buffer = handle->readbuf;
  int cursor = handle->readbufcrs;
  char fdelim = handle->opts->field_delimiter;

  /* First character of the file? */
  if (cursor == 0)
    return 1;

  /* Last character of the file? */
  if (buffer[cursor + 1] == '\0')
    return 0;

  if (handle->in_text && on_text_delimiter(handle)) {
    /* An opening text delimiter is the start of a field */
    return 1;
  }
  else if (handle->in_text) {
    /* If the cursor is between text delimiters but not on the opening text
     * delimiter, this can't be the start of a field */
    return 0;
  }
  else {
    /* Previous character was a field delimiter? */
    if (buffer[cursor - 1] == fdelim)
      return 1;

    /* Previous character was a line break? */
    if (is_line_break(buffer[cursor - 1], handle))
      return 1;
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
