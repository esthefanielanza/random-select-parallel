#include "input.h"

void readInput(char *argv[], int *n, int *i, char **type, int *threads){
  *n = atoi(argv[1]);
  *i = atoi(argv[2]);
  *type = argv[3];
  *threads = atoi(argv[4]);
}
