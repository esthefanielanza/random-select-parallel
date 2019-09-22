#include <time.h>
#include <omp.h>

#include "input.h"

void swap(int *A, int i, int j) {
  if(i != j){
    int temp = A[i];
    A[i] = A[j];
    A[j] = temp;
  }
}

void sumPrefixes(int *prefixSumL, int *prefixSumR, int *bitsL, int* bitsR, int size) {
  int i;

  prefixSumL[0] = bitsL[0];
  prefixSumR[0] = bitsR[0];

  for(i = 1; i < size - 1; i++) {
    prefixSumL[i] = prefixSumL[i-1] + bitsL[i];
    prefixSumR[i] = prefixSumR[i-1] + bitsR[i];
  }
}

typedef struct Partition {
  int *R;
  int sizeR;
  int *L;
  int sizeL;
} Partition;

Partition *partition(int *A, int size, int pivot) {
  int j;

  int *bitsL = (int *)calloc(size, sizeof(int));
  int *bitsR = (int *)calloc(size, sizeof(int));

  int *prefixSumL = (int *)calloc(size, sizeof(int));
  int *prefixSumR = (int *)calloc(size, sizeof(int));


  for(j = 0; j < size - 1; j++) {
    if(A[j] < pivot) {
      bitsL[j] = 1;
    } else {
      bitsR[j] = 1;
    }
  }
  
  sumPrefixes(prefixSumL, prefixSumR, bitsL, bitsR, size);

  int sizeL = prefixSumL[size - 1];
  int sizeR = prefixSumR[size - 1];

  int *L = (int *)calloc(sizeL, sizeof(int));
  int *R = (int *)calloc(sizeR, sizeof(int));

  for(j = 0; j < sizeL; j++) {
    L[prefixSumL[bitsL[j]]] = A[j];
  }

  for(j = 0; j < sizeR; j++) {
    R[prefixSumR[bitsL[j]]] = A[j];
  }

  Partition *result = (Partition *)malloc(1 * sizeof(Partition));
  result->L = L;
  result->sizeL = sizeL;
  
  result->R = R;
  result->sizeR = sizeR;

  return result;
}

int randomizedSelect(int *A, int size, int i) {
  int x = rand() % size;
  int pivot = A[x];

  Partition *result = partition(A, size, pivot);

  printf("Partição da esquerda (%d)\n", result->sizeL);
  for(int i = 0; i < result->sizeL; i++) {
    printf("%d ", result->L[i]);
  }

  printf("Partição da direita (%d)\n", result->sizeR);
  for(int i = 0; i < result->sizeR; i++) {
    printf("%d ", result->R[i]);
  }

  if(result->sizeL == i - 1) {
    return pivot;
  }

  // if(result->sizeL > i) {
  //   return randomizedSelect(result->L, result->sizeL, i);
  // } else {
  //   return randomizedSelect(result->R, result->sizeR, i - result->sizeL - 1);
  // }

  printf("\n");
  return 0;
}

int comparer (const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

int main (int argc, char *argv[]) {
  srand(time(NULL));
  int n, i, threads, j;
  char *type;
  double startTime, endTime;

  readInput(argv, &n, &i, &type, &threads);

  int *A = (int*)malloc(n * sizeof(int));

  for(j = 0; j < n; j++){
    A[j] = rand() % 100;
  }

  startTime = omp_get_wtime();

  int result = randomizedSelect(A, n, i);

  endTime = omp_get_wtime();

  if(type[0] == 'a') {
    qsort(A, n, sizeof(int), comparer);
    for(j = 0; j < n; j++){
      printf("%d ", A[j]);
    }

    printf("\n%d\n", result);
  }
 
  printf("%f\n", endTime - startTime);
  (void)argc;
  free(A);
	return 0;
}