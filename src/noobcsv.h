
#ifndef NOOBCSV_H
#define NOOBCSV_H

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

typedef struct NoobCSVOptions_ NoobCSVOptions;

/*
 * Sets appropriate defaults to each field.
 *
 * If you don't plan on filling every field manually, you should use this
 * function.
 */
void noobcsv_init_opts(NoobCSVOptions* opts);

#endif
