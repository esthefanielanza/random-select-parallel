#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void exchange(int *A, int i, int j) {
  if(i != j){
    int temp = A[i];
    A[i] = A[j];
    A[j] = temp;
  }
}

int partition(int *A, int p, int r) {
  int size = r-p+1;

  int i = p - 1;
  int x = p + (rand() % size);
  int pivot = A[x];

  exchange(A, x, r);
  for(x = p; x < r; x++) {
    if(A[x] <= pivot) {
      i++;
      exchange(A, i, x);
    }
  }

  i++;
  exchange(A, i, r);

  return i;
}

int randomizedPartition(int *A, int p, int r) {
  int i = rand() % r + 1;
  exchange(A, r, i);
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

int main (void) {
  srand(time(NULL));
  int n = 0;
  int *A = (int*)malloc(5 * sizeof(int));

  for(n = 0; n < 5; n++){
    A[n] = rand() % 100;
  }

  qsort(A, 5, sizeof(int), comparer);
  for(n = 0; n < 5; n++){
    printf("%d ", A[n]);
  }

  printf("\n");

  int result = randomizedSelect(A, 0, 4, 4);

  printf("%d\n", result);
  free(A);
	return 0;
}