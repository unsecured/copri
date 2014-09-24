// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include "pool.h"

#define POOL_DEFAULT_SIZE 256

#define POOL_REALLOC_SIZE 256

void pool_init(mpz_pool *p, size_t size) {
	size_t i;
	if (size < 1) size = POOL_DEFAULT_SIZE;
	p->array = (mpz_t*)malloc(size * sizeof(mpz_t));
	p->used = 0;
	p->size = size;
	for (i=0; i<size; i++) {
		mpz_init(p->array[i]);
	}
}

void pool_clear(mpz_pool *p) {
	size_t i;
	if (p->used > 0) {
		fprintf(stderr, "Can not clear pool %lx: %zu integers in use!\n", (intptr_t)p, p->used);
	} else {
		for (i=0; i<p->size; i++) {
			mpz_clear(p->array[i]);
		}
		free(p->array);
		p->array = NULL;
		p->used = p->size = 0;
	}
}

void pool_pop(mpz_pool *p, mpz_t ret) {
	size_t i;
	if (p->used >= p->size) {
		fprintf(stderr, "Pool %lx to small!\n", (intptr_t)p);
		p->array = (mpz_t *)realloc(p->array, (p->size + POOL_REALLOC_SIZE) * sizeof(mpz_t));
		for (i=p->size; i<(p->size + POOL_REALLOC_SIZE); i++) {
			mpz_init(p->array[i]);
		}
		p->size += POOL_REALLOC_SIZE;
	}
	mpz_swap(p->array[p->used++], ret);
}

void pool_push(mpz_pool *p, mpz_t i) {
	if (p->used <= 0) {
		fprintf(stderr, "Can not push integer pool %lx is full!\n", (intptr_t)p);
	} else {
		mpz_swap(p->array[--p->used], i);
	}
}
