
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
  NOOBCSV_CT_TEXT,
  NOOBCSV_CT_FDELIM,
  NOOBCSV_CT_TDELIM_OPEN,
  NOOBCSV_CT_TDELIM_CLOSE,
  NOOBCSV_CT_LINE_BREAK,
  NOOBCSV_CT_EOF
} noobcsv_ct;

/* noobcsv text delimiter types */
typedef enum {
  NOOBCSV_TDT_OPENING = 1,
  NOOBCSV_TDT_CLOSING,
  NOOBCSV_TDT_ESCAPED
} noobcsv_tdt;

#endif
