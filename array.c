// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include "array.h"
#include "config.h"


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

// Copy the elements from `src` to `target`.
void array_copy(mpz_array *target, mpz_array *src) {
	size_t i;
	if ((target->size - target->used) < src->size) {
		target->size = src->size + target->used;
		target->array = (mpz_t *)realloc(target->array, target->size * sizeof(mpz_t));
	}
	for (i = 0; i < src->used; i++) {
		mpz_init_set(target->array[target->used++], src->array[i]);
	}
}

// Adds an `integer` to the end of the array. This function expects an **initialized** integer and does an deep copy.
void array_add(mpz_array *a, const mpz_t integer) {
	if (a->used == a->size) {
		a->size *= 2;
		a->array = (mpz_t *)realloc(a->array, a->size * sizeof(mpz_t));
	}
	mpz_init_set(a->array[a->used++], integer);
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

// ## load, store & print a array

// Populates an array with values read from a file.
size_t array_of_file(mpz_array *a, const char *filename) {
	size_t count = 0;
	mpz_t i;
	if (access(filename, R_OK) == -1) return 0;
	FILE *in = fopen(filename, "r");
	mpz_init(i);
	while(mpz_inp_raw(i, in) > 0) {
		array_add(a, i);
		count++;
	}
	mpz_clear(i);
	fclose(in);
	return count;
}

// **TODO**
int array_to_file(mpz_array *a, const char *filename) {
	return 0;
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
