
#include "noobcsv.h"

/*
 * Structure for holding options about reading and writing CSV files.
 */
struct NoobCSVOptions_ {
  int first_line;
  int last_line;
  int line_count;

  char field_delimiter;
  char text_delimiter;
  char line_endings;

  int auto_line_endings;
};

void noobcsv_init_opts(NoobCSVOptions* opts) {
  opts->first_line = -1;
  opts->last_line = -1;
  opts->line_count = -1;

  opts->field_delimiter = ',';
  opts->text_delimiter = '"';
  opts->line_endings = '\n';

  opts->auto_line_endings = 1;
}
