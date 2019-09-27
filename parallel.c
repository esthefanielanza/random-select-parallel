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
  int pivot;

  Interval *interval;
} ChangeSidesData;

typedef struct ThreadOperation {
  ChangeSidesData *data;
  Interval *interval;
} ThreadOperation;

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

Partition *partition(int *A, int size, int pivot, void * pool, void * pool2, int threads) {
  int j;

  ChangeSidesData *changeSidesData = (ChangeSidesData *)malloc(1 * sizeof(ChangeSidesData)); 

  changeSidesData->A = A;
  changeSidesData->pivot = pivot;

  changeSidesData->bitsL = (int *)calloc(size, sizeof(int));
  changeSidesData->bitsR = (int *)calloc(size, sizeof(int));

  changeSidesData->prefixSumL = (int *)calloc(size, sizeof(int));
  changeSidesData->prefixSumR = (int *)calloc(size, sizeof(int));


  int sizeOfInterval = size/threads;

  for(j = 0; j < threads; j++) {
    ThreadOperation *threadData = (ThreadOperation *)malloc(1 * sizeof(ThreadOperation)); 
    threadData->interval = (Interval *)malloc(1 * sizeof(Interval)); 
    threadData->data = changeSidesData;

    threadData->interval->start = j * sizeOfInterval;

    if(j == threads - 1) {
      threadData->interval->end = size;
    } else {
      threadData->interval->end = threadData->interval->start + sizeOfInterval;
    }

    pool_enqueue(pool2, (void *)threadData);
  }

  pool_wait(pool2);

  sumPrefixes(changeSidesData->prefixSumL, changeSidesData->prefixSumR, changeSidesData->bitsL, changeSidesData->bitsR, size);

  int sizeL = changeSidesData->prefixSumL[size - 1];
  int sizeR = changeSidesData->prefixSumR[size - 1];

  changeSidesData->L = (int *)calloc(sizeL, sizeof(int));
  changeSidesData->R = (int *)calloc(sizeR, sizeof(int));

  for(j = 0; j < threads; j++) {
    ThreadOperation *threadData = (ThreadOperation *)malloc(1 * sizeof(ThreadOperation)); 
    threadData->interval = (Interval *)malloc(1 * sizeof(Interval)); 
    threadData->data = changeSidesData;

    threadData->interval->start = j * sizeOfInterval;
    if(j == threads - 1) {
      threadData->interval->end = size;
    } else {
      threadData->interval->end = threadData->interval->start + sizeOfInterval;
    }

    pool_enqueue(pool, (void *)threadData);
  }

  pool_wait(pool);

  Partition *result = (Partition *)malloc(1 * sizeof(Partition));
  result->L = changeSidesData->L;
  result->sizeL = sizeL;
  
  result->R = changeSidesData->R;
  result->sizeR = sizeR;

  return result;
}

int randomizedSelect(int *A, int size, int i, void * pool, void * pool2, int threads) {

  int x = rand() % size;

  int pivot = A[x];
  
  Partition *result = partition(A, size, pivot, pool, pool2, threads);

  if(result->sizeL == i - 1) {
    return pivot;
  }

  if(result->sizeL == 0 && result->sizeR == 0) {
    return pivot;
  }

  if(result->sizeL >= i) {
    return randomizedSelect(result->L, result->sizeL, i, pool, pool2, threads);
  } else {
    return randomizedSelect(result->R, result->sizeR, i - result->sizeL - 1, pool, pool2, threads);
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

void changeSides(ThreadOperation *d) {
  int i;

  for(i = d->interval->start; i < d->interval->end; i++) {
    if(d->data->bitsL[i] == 1) {
      d->data->L[d->data->prefixSumL[i] - 1] = d->data->A[i];
    }

    if(d->data->bitsR[i] == 1) {
      d->data->R[d->data->prefixSumR[i] - 1] = d->data->A[i];
    }
  }
}

void calculateBits(ThreadOperation *d) {
  int i;
  for(i = d->interval->start; i < d->interval->end; i++) {
    if(d->data->A[i] < d->data->pivot) {
      d->data->bitsL[i] = 1;
    }

    else if(d->data->A[i] > d->data->pivot) {
      d->data->bitsR[i] = 1;
    }
  }
}

int main (int argc, char *argv[]) {
  srand(time(NULL));
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
  shuffle(A, n);

  startTime = omp_get_wtime();

  void * pool = pool_start((void *)(*changeSides), threads);
  void * pool2 = pool_start((void *)(*calculateBits), threads);
 
  int result = randomizedSelect(A, n, i, pool, pool2, threads);

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
  pool_end(pool2);
  free(A);
	return 0;
}