#ifndef LOCATION_H
#define LOCATION_H

typedef struct __location {
  const char *filename;
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} location_t;

#endif  // LOCATION_H
