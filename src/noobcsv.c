
#include <stdlib.h>   /* malloc */

#include "noobcsv.h"
#include "noobcsv_private.h"

struct NoobCSVOptions noobcsv_create_opts()
{
  struct NoobCSVOptions opts;

  opts.field_delimiter = ',';
  opts.text_delimiter = '"';
  opts.line_endings = '\n';

  opts.auto_line_endings = 1;

  return opts;
}

NoobCSVHandle *noobcsv_create_handle(FILE *file, struct NoobCSVOptions *opts)
{
  return noobcsv_create_handle_bs(file, opts, NOOBCSV_BUFSIZE);
}

NoobCSVHandle *noobcsv_create_handle_bs(FILE *file,
                                        struct NoobCSVOptions *opts,
                                        ssize_t bufsize)
{
  NoobCSVHandle *handle = malloc(sizeof(NoobCSVHandle));

  handle->opts = opts;
  handle->file = file;
  handle->bufsize = bufsize;
  handle->readbufcrs = -1;
  handle->in_text = 0;

  handle->readbuf = malloc(sizeof(char) * bufsize);

  return handle;
}

void noobcsv_free_handle(NoobCSVHandle *handle)
{
  free(handle->readbuf);
  free(handle);
}
