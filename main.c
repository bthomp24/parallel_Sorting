#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>

static const long Num_To_Sort = 1000000000;

int partition(int *arr, int low, int high) {
    int pivot = arr[high];
    int i = low;
    for (int j = low; j < high; j++)
    {
        if(arr[j] < pivot) {
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
        }
    }
    int temp = arr[i];
    arr[i] = arr[high];
    arr[high] = temp;
    return i;
}

// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section
void sort_s(int *arr, int low, int high) {
    // implements quick sort on the array
    int i;
    if (low < high) {
        i = partition(arr, low, high);
        sort_s(arr, low, i - 1);
        sort_s(arr, i + 1, high);
    }
}

// Parallel version of your sort
void sort_p(int *arr, int low, int high) {
    // implements quick sort on the array
    // Based on help from https://codereview.stackexchange.com/questions/181441/making-a-faster-parallel-quicksort
    int i;
    if (low < high){
        i = partition(arr, low, high);
        #pragma omp task
        sort_p(arr, low, i - 1);
        #pragma omp task
        sort_p(arr, i + 1, high);

    }

}

int main() {
    int *arr_s = malloc(sizeof(int) * Num_To_Sort);
    long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            arr_s[i] = rand_r(&seed);
        }
    }

    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.
    int *arr_p = malloc(sizeof(int) * Num_To_Sort);
    memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    sort_s(arr_s, 0, Num_To_Sort - 1);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_s);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    #pragma omp parallel
    {
    #pragma omp single
        sort_p(arr_p, 0, Num_To_Sort - 1);
    };
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_p);

    return 0;
}


