#include <stdio.h>
#include <stdlib.h>

void exchange(int *A, int i, int j) {
  int temp = A[i];
  A[i] = A[j];
  A[j] = temp;
}

int partition(int *A, int p, int r) {
  int j;
  int x = A[r];
  int i = p - 1;

  for(j = p; j < r - 1; j++) {
    if(A[j] < x) {
      exchange(A, i, j);
    }
  }

  exchange(A, i + 1, j);
  return i + 1;
}

int randomizedPartition(int *A, int p, int r) {
  int i = RANDOM(p, r);
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
    randomizedSelect(A, p, q - 1, i);
  } else {
    randomizedSelect(A, q + 1, r, i - k);
  }

  return 0;
}

int main (void) {
  int A[7] = {7,6,2,3,1,2,0};
  int result = randomizedSelect(A, 0, 7, 3);
  printf("%d", result);
	return 0;
}
