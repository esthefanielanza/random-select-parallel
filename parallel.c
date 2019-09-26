#include <time.h>
#include <omp.h>

#include "input.h"
#include "pthreadPool.h"

typedef struct Interval {
  int start;
  int end;
} Interval;

typedef struct ChangeSidesData {
  int *bitsL;
  int *L;
  int *prefixSumL;
  
  int *bitsR;
  int *R;
  int *prefixSumR;

  int *A;
  int idx;

  Interval *interval;
} ChangeSidesData;

typedef struct Partition {
  int *R;
  int sizeR;
  int *L;
  int sizeL;
} Partition;

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

  for(i = 1; i < size; i++) {
    prefixSumL[i] = prefixSumL[i-1] + bitsL[i];
    prefixSumR[i] = prefixSumR[i-1] + bitsR[i];
  }
}

Partition *partition(int *A, int size, int pivot, void * pool, int threads) {
  int j;

  ChangeSidesData *changeSidesData = (ChangeSidesData *)malloc(1 * sizeof(ChangeSidesData)); 

  changeSidesData->A = A;

  changeSidesData->bitsL = (int *)calloc(size, sizeof(int));
  changeSidesData->bitsR = (int *)calloc(size, sizeof(int));

  changeSidesData->prefixSumL = (int *)calloc(size, sizeof(int));
  changeSidesData->prefixSumR = (int *)calloc(size, sizeof(int));


  for(j = 0; j < size; j++) {
    if(A[j] < pivot) {
      changeSidesData->bitsL[j] = 1;
    }

    else if(A[j] > pivot) {
      changeSidesData->bitsR[j] = 1;
    }
  }

  sumPrefixes(changeSidesData->prefixSumL, changeSidesData->prefixSumR, changeSidesData->bitsL, changeSidesData->bitsR, size);
  
  // printf("\nPrefixSum L\n");
  // for(j = 0; j < size; j++) {
  //   printf("%d ", changeSidesData->prefixSumL[j]);
  // }
  // printf("\n");

  // printf("\nPrefixSum R\n");
  // for(j = 0; j < size; j++) {
  //   printf("%d ", changeSidesData->prefixSumR[j]);
  // }
  // printf("\n");

  int sizeL = changeSidesData->prefixSumL[size - 1];
  int sizeR = changeSidesData->prefixSumR[size - 1];

  changeSidesData->L = (int *)calloc(sizeL, sizeof(int));
  changeSidesData->R = (int *)calloc(sizeR, sizeof(int));

  int sizeOfInterval = size/threads;

  for(j = 0; j < threads; j++) {  
    changeSidesData->interval = (Interval *)malloc(1 * sizeof(Interval)); 

    changeSidesData->interval->start = j * sizeOfInterval;
    // printf("start: %d", changeSidesData->interval->start);
    if(j == threads - 1) {
      changeSidesData->interval->end = changeSidesData->interval->start + sizeOfInterval;
    } else {
      changeSidesData->interval->end = size - changeSidesData->interval->start;
    }
    // printf("end: %d", changeSidesData->interval->end);

    pool_enqueue(pool, (void *)changeSidesData, (char)0);
  }

  pool_wait(pool);


  // printf("\nL side\n");
  // for(j = 0; j < sizeL; j++) {
  //   printf("%d ", changeSidesData->L[j]);
  // }
  // printf("\n");

  Partition *result = (Partition *)malloc(1 * sizeof(Partition));
  result->L = changeSidesData->L;
  result->sizeL = sizeL;
  
  result->R = changeSidesData->R;
  result->sizeR = sizeR;

  return result;
}

int randomizedSelect(int *A, int size, int i, void * pool, int threads) {
  // int j;
  // printf("============");
  // printf("Array!\n");
  // printf("size %d\n", size);
  // for(j = 0; j < size; j++){
  //   printf("%d ", A[j]);
  // };

  int x = rand() % size;

  // printf("x: %d", x);
  int pivot = A[x];

  // printf("Pivot %d\n\n", pivot);
  
  Partition *result = partition(A, size, pivot, pool, threads);

  // printf("Partição da esquerda (%d)\n", result->sizeL);
  // for(int j = 0; j < result->sizeL; j++) {
  //   printf("%d ", result->L[j]);
  // }
  // printf("\n\n");

  // printf("Partição da direita (%d)\n", result->sizeR);
  // for(int j = 0; j < result->sizeR; j++) {
  //   printf("%d ", result->R[j]);
  // }
  // printf("\n");

  // printf("\nTamanho L %d\n", result->sizeL);
  // printf("I: %d\n", i);
  // printf("\n");

  if(result->sizeL == i - 1) {
    // printf("\n returning pivo\n");
    return pivot;
  }

  if(result->sizeL == 0 && result->sizeR == 0) {
    return pivot;
  }

  if(result->sizeL >= i) {
    // printf("ESQUERDA");
    return randomizedSelect(result->L, result->sizeL, i, pool, threads);
  } else {
    // printf("DIREITA (%d)", result->sizeR);
    return randomizedSelect(result->R, result->sizeR, i - result->sizeL - 1, pool, threads);
  }
}

void shuffle(int *A, int n) {
  int i;

  for(i = 0; i < n; i++) {
    swap(A, i, rand() % n);
  }
}

int comparer (const void * a, const void * b) {
	return ( *(int*)a - *(int*)b );
}

void changeSides(ChangeSidesData *d) {
  int i;

  for(i = d->interval->start; i < d->interval->end; i++) {
    // printf("changing %d", i);

    if(d->bitsL[i] == 1) {
      d->L[d->prefixSumL[i] - 1] = d->A[i];
    }

    if(d->bitsR[i] == 1) {
      d->R[d->prefixSumR[i] - 1] = d->A[i];
    }
  }
}

int main (int argc, char *argv[]) {
  srand(0);
  int n, i, threads, j, aux;
  char *type;
  double startTime, endTime;

  readInput(argv, &n, &i, &type, &threads);

  int *A = (int*)malloc(n * sizeof(int));

  aux = 0;
  for(j = 0; j < n; j++){
    aux += 1 + rand() % 10;
    A[j] = aux;
  }
  printf("\nCorrect answer: %d\n", A[i - 1]);
  shuffle(A, n);

  printf("\n");

  startTime = omp_get_wtime();

  void * pool = pool_start((void *)(*changeSides), threads);

  int result = randomizedSelect(A, n, i, pool, threads);

  endTime = omp_get_wtime();

  if(type[0] == 'a') {
    for(j = 0; j < n; j++){
      printf("%d ", A[j]);
    }

    printf("\n%d\n", result);
  }
 
  printf("%f\n", endTime - startTime);
  (void)argc;
  pool_end(pool);
  free(A);
	return 0;
}