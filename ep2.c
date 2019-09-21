#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void exchange(int A[], int i, int j) {
  int temp = A[i];
  A[i] = A[j];
  A[j] = temp;
}

int partition(int A[], int p, int r) {
  int j;
  int x = A[r];
  int i = p - 1;

  for(j = p; j < r; j++) {
    if(A[j] <= x) {
      i++;
      exchange(A, i, j);
    }
  }
  
  i++;
  exchange(A, i, r);

  return i;
}

int randomizedPartition(int A[], int p, int r) {
  int i = rand() % r + 1;
  exchange(A, r, i);
  return partition(A, p, r);
}

int randomizedSelect(int A[], int p, int r, int i) {
  if(p == r) {
    return A[p];
  }

  int q = randomizedPartition(A, p, r);
  // printf("%d\n", q);
  int k = q - p + 1;

  if(i == k) {
    return A[q];
  } else if( i < k ) {
    return randomizedSelect(A, p, q - 1, i);
  } else {
    return randomizedSelect(A, q + 1, r, i - k);
  }
}

int main (void) {
  srand(time(NULL));
  int n = 0;
  int A[10];

  for(n = 0; n < 10; n++) {
    A[n] = rand() % 100;
    printf("%d ", A[n]);
  }

  printf("\n");

  int result = randomizedSelect(A, 0, 9, 8);

  printf("%d\n", result);
	return 0;
}
