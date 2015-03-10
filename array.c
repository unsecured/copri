// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gmp.h>
#include "array.h"
#include "config.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// # array auxiliary
//
// An minimal dynamic sized array.
//
// The struct `mpz_array` is in `copri.h` defined as follows:
//
//     typedef struct {
//        mpz_t * array;
//        size_t used;
//        size_t size;
//     } mpz_array;
//
// See [array test](test-array.html) for basic usage.

#define ARRAY_DEFAULT_SIZE 256

// Initialize the array with an capacity of `size`.
void array_init(mpz_array *a, size_t size) {
	if (size < 1) size = ARRAY_DEFAULT_SIZE;
	a->array = (mpz_t *)malloc(size * sizeof(mpz_t));
	a->used = 0;
	a->size = size;
}

// Adds an `integer` to the end of the array. This function expects an **initialized** integer and does an deep copy.
void array_add(mpz_array *a, const mpz_t integer) {
	if (a->used == a->size) {
		a->size *= 2;
		a->array = (mpz_t *)realloc(a->array, a->size * sizeof(mpz_t));
	}
	mpz_init_set(a->array[a->used++], integer);
}

// Adds the elements from `src` to `target`.
void array_add_array(mpz_array *target, mpz_array *src) {
	size_t i;
	if ((target->size - target->used) < src->size) {
		target->size = src->size + target->used;
		target->array = (mpz_t *)realloc(target->array, target->size * sizeof(mpz_t));
	}
	for (i = 0; i < src->used; i++) {
		mpz_init_set(target->array[target->used++], src->array[i]);
	}
}

// Copy the elements from `src` to `target`.
// First `target` is cleared and the elements from `src` are added to `target`.
void array_copy(mpz_array *target, mpz_array *src) {
	array_clear(target);
	array_init(target, src->used);
	array_add_array(target, src);
}

// Frees the memory of the array. This function expects initialized integers in the whole array.
void array_clear(mpz_array *a) {
	size_t i;
	for (i = 0; i < a->used; i++) {
		mpz_clear(a->array[i]);
	}
	free(a->array);
	a->array = NULL;
	a->used = a->size = 0;
}

// Swap the elements from `a` and `b`.
int array_swap(mpz_array *a, mpz_array *b) {
	size_t i;
	if (a->used == b->used) {
		for (i = 0; i < a->used; i++) {
			mpz_swap(a->array[i], b->array[i]);
		}
		return 0;
	} else {
		return 1;
	}
}

// Tests if the two arrays are equal.
// Use `array_msort` to ensure the same ordering.
// Return `1` if the arrays are equal and `0` if they differ
int array_equal(mpz_array *a, mpz_array *b) {
	size_t i;
	if (a->used == b->used) {
		for (i = 0; i < a->used; i++) {
			if (mpz_cmp(a->array[i], b->array[i]) != 0)
				return 0;
		}
		return 1;
	} else {
		return 0;
	}
}

// Print a `mpz_array` in base 10 to stdout.
void array_print(mpz_array *a) {
	size_t i;
	char* str;
	for (i = 0; i < a->used; i++) {
		str = mpz_get_str(NULL, 10, a->array[i]);
		printf("%zu: %s\n", i, str);
		free(str);
	}
}

// ## load, store & print a array

// Populates an array with values read from stream `in`.
size_t array_of_stdio(mpz_array *a, FILE *in) {
	size_t count = 0;
	mpz_t buf;
	mpz_init(buf);
	while(mpz_inp_raw(buf, in) > 0) {
		array_add(a, buf);
		count++;
	}
	mpz_clear(buf);
	return count;
}


// Populates an array with values read from a file.
size_t array_of_file(mpz_array *a, const char *filename) {
	size_t count;
	FILE *in;
	if (strcmp(filename, "-") == 0) {
		in = stdin;
	} else {
		if (access(filename, R_OK) != 0) return 0;
		in = fopen(filename, "r");
	}
	count = array_of_stdio(a, in);
	if (strcmp(filename, "-") != 0)
		fclose(in);
	return count;
}

// write the content of the array to stream
size_t array_to_stdio(mpz_array *a, FILE *out) {
	size_t i, count = 0;
	for (i = 0; i < a->used; i++) {
		if (mpz_out_raw(out, a->array[i]) > 0) {
			count++;
		}
	}
	return count;
}


// Store the content of the array in a file
size_t array_to_file(mpz_array *a, const char *filename) {
	size_t count;
	FILE *out;
	if (strcmp(filename, "-") == 0) {
		out = stdout;
	} else {
		out = fopen(filename, "a+");
	}
	count = array_to_stdio(a, out);
	if (strcmp(filename, "-") != 0)
		fclose(out);
	return count;
}

// ## sort a array

// merge two sorted lists
void array_msort_merge(mpz_array *a, size_t iLeft, size_t iRight, size_t iEnd, mpz_array *b) {
	int i0 = iLeft;
	int i1 = iRight;
	int j;

	// While there are elements in the left or right lists
	for (j = iLeft; j < iEnd; j++) {
		// If left list head exists and is <= existing right list head
		if (i0 < iRight && (i1 >= iEnd || mpz_cmp(a->array[i0], a->array[i1]) <= 0)) {
			mpz_set(b->array[j], a->array[i0]);
			i0 = i0 + 1;
		} else {
			mpz_set(b->array[j], a->array[i1]);
			i1 = i1 + 1;
		}
	}
}

// Sort the array by [merge sort](http://en.wikipedia.org/wiki/Merge_sort)
void array_msort(mpz_array *a) {
	size_t width, i, n = a->used;
	mpz_array b;

	// Prepare the working array B.
	array_init(&b, n);
	b.used = n;
	for (i=0; i<n; i++) {
		mpz_init(b.array[i]);
	}

	// Each 1-element run in A is already *sorted*.
	// Make successively longer sorted runs of length 2, 4, 8, 16... until whole array is sorted.
	for (width = 1; width < n; width = 2 * width) {
		// Array A is full of runs of length width.
		for (i = 0; i < n; i = i + 2 * width) {
			// Merge two runs: A[i:i+width-1] and A[i+width:i+2*width-1] to B[]
			// or copy A[i:n-1] to B[] ( if(i+width >= n) )
			array_msort_merge(a, i, MIN(i+width, n), MIN(i+2*width, n), &b);
		}

		// Now work array B is full of runs of length 2*width.
		// Swap array B to array A for next iteration.
		array_swap(a, &b);
		// Now array A is full of runs of length 2*width.
	}
	array_clear(&b);
}

// Test if the array contains the integer.
int array_contains(mpz_array *a, const mpz_t integer) {
	size_t i;
	for (i = 0; i < a->used; i++) {
		if (mpz_cmp(a->array[i], integer) == 0)
			return 1;
	}
	return 0;
}

// Returns an sorted array with unique values.
// This function expects an sorted array as input.
void array_unique(mpz_array *uniques, mpz_array *sorted) {
	size_t i;
	for (i = 0; i < sorted->used; i++) {
		if ((i + 1) == sorted->used) {
			array_add(uniques, sorted->array[i]);
		} else {
			if (mpz_cmp(sorted->array[i], sorted->array[i+1]) != 0) {
				array_add(uniques, sorted->array[i]);
			}
		}
	}
}
