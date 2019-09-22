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

int partition(int *A, int p, int r) {
  int size = r-p+1;

  int i = p - 1;
  int x = p + (rand() % size);
  int pivot = A[x];
  int j;

  swap(A, x, r);

  int *bitsL = (int *)calloc(size, sizeof(int));
  int *bitsR = (int *)calloc(size, sizeof(int));

  int *L = (int *)calloc(size, sizeof(int));
  int *R = (int *)calloc(size, sizeof(int));

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

  // Paralelizar essa parte aqui!
  for(j = 0; j < size; j++) {
    L[prefixSumL[bitsL[j]]] = A[j];
    R[prefixSumR[bitsR[j]]] = A[j];
  }

  return i;
}

int randomizedPartition(int *A, int p, int r) {
  int i = rand() % r + 1;
  swap(A, r, i);
  return partition(A, p, r);
}

int randomizedSelect(int *A, int p, int r, int i) {
  if(p == r) {
    return A[p];
  }

  int q = randomizedPartition(A, p, r);
  int k = q - p + 1;

  if(i == k) {
    return A[q];
  } else if( i < k ) {
    return randomizedSelect(A, p, q - 1, i);
  } else {
    return randomizedSelect(A, q + 1, r, i - k);
  }
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

  int result = randomizedSelect(A, 0, n-1, i);

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