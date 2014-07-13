
#ifndef NOOBCSV_PRIVATE_H
#define NOOBCSV_PRIVATE_H

#include "noobcsv.h"

/* Structure for managing a CSV reading/writing session */
struct NoobCSVHandle_ {
  struct NoobCSVOptions *opts;
  FILE *file;

  /* The buffer size, read buffer and read buffer cursor */
  ssize_t bufsize;
  char *readbuf;
  int readbufcrs;

  /* Whether or not the read cursor is currently between text delimiters */
  short in_text;
};

/* noobcsv character types */
typedef enum {
  NOOBCSV_TEXT,
  NOOBCSV_FDELIM,
  NOOBCSV_TDELIM_OPEN,
  NOOBCSV_TDELIM_CLOSE,
  NOOBCSV_LINE_BREAK,
  NOOBCSV_EOF
} noobcsv_ct;

/* noobcsv text delimiter types */
typedef enum {
  NOOBCSV_TD_OPEN = 1,
  NOOBCSV_TD_CLOSE,
  NOOBCSV_TD_ESCAPED
} noobcsv_tdt;

#endif
