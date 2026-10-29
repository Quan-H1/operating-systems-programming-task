/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
    int leftIndex = leftstart;
    int rightIndex = rightstart;
    int bIndex = leftstart;  
	int i;
    while (leftIndex <= leftend && rightIndex <= rightend) {
		if (A[leftIndex] <= A[rightIndex]) {
			B[bIndex++] = A[leftIndex++];
		} else {
			B[bIndex++] = A[rightIndex++];
		}
    }

    while (leftIndex <= leftend) B[bIndex++] = A[leftIndex++];
    while (rightIndex <= rightend) B[bIndex++] = A[rightIndex++];

    for (i = leftstart; i <= rightend; i++) {
        A[i] = B[i];
    }
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
    if (left >= right) return;

	int mid = (left+right)/2;
	my_mergesort(left,mid);
	my_mergesort(mid+1,right);
	merge(left,mid,mid+1,right);
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
	struct argument *a = (struct argument*)arg;
	int left = a->left;
	int right = a->right;
	int level = a->level;
	
	if (level >= cutoff || left >= right) {
		my_mergesort(left, right);
		return NULL;
	}

	int mid = (left+right)/2;

	pthread_t leftThread, rightThread;
	
    struct argument *leftArg = buildArgs(left, mid, level+1);
    struct argument *rightArg = buildArgs(mid+1, right, level+1);

	pthread_create(&leftThread, NULL, parallel_mergesort, leftArg);
	pthread_create(&rightThread, NULL, parallel_mergesort, rightArg);
	
    pthread_join(leftThread, NULL);
    pthread_join(rightThread, NULL);

	free(leftArg);
	free(rightArg);

    merge(left, mid, mid + 1, right);

	return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level) {
    struct argument *arg = malloc(sizeof(struct argument));
    arg->left = left;
    arg->right = right;
    arg->level = level;
    return arg;
}