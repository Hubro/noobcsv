
#ifndef NOOBCSV_H
#define NOOBCSV_H

#include <stdio.h>

#define NOOBCSV_BUFSIZE 1024

/***************************************************************************
 *                            CORE DECLARATIONS                            *
 ***************************************************************************/

/* Abstract datatype for a noobcsv read/write session */
typedef struct NoobCSVHandle_ NoobCSVHandle;

/* Structure for holding options about reading and writing CSV files */
struct NoobCSVOptions {
  char field_delimiter;
  char text_delimiter;
  char record_delimiter;

  int auto_line_endings;
};

/* Creates and returns a NoobCSVOptions struct with reasonable defaults */
struct NoobCSVOptions noobcsv_create_opts();

/* Creates and returns a NoobCSVHandle struct */
NoobCSVHandle *noobcsv_create_handle(FILE *file, struct NoobCSVOptions *opts);

/* Creates and returns a NoobCSVHandle struct with a custom buffer size */
NoobCSVHandle *noobcsv_create_handle_bs(
  FILE *file, struct NoobCSVOptions *opts, ssize_t bufsize
);

/* Change the buffer size. MUST ONLY BE DONE RIGHT AFTER HANDLE CREATION.
 * Returns 1 on success, 0 if "realloc" failed. */
int noobcsv_set_bufsize(NoobCSVHandle *handle, ssize_t new_size);

void noobcsv_free_handle(NoobCSVHandle *handle);

/***************************************************************************
 *                       READ-SPECIFIC DECLARATIONS                        *
 ***************************************************************************/

/* Moves the file position indicator to the start of the next field */
int noobcsv_next_field(NoobCSVHandle *handle);

/* Moves the file position indicator to the start of the next record */
int noobcsv_next_record(NoobCSVHandle *handle);

/* Reads from the current field. Returns the amount of characters read */
int noobcsv_read(NoobCSVHandle *handle, char* buffer, int buffer_size);

/***************************************************************************
 *                       WRITE-SPECIFIC DECLARATIONS                       *
 ***************************************************************************/

/* Nothing here yet... */

#endif
