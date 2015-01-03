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
	p->init_bit_size = 1048576; // 1024*1024
#if INSPECT_POOL
	p->push_count = 0;
	p->small_count = 0;
	p->max_used = 0;
	mpz_init_set_ui(p->push_sum, 0);
#endif
	for (i=0; i<size; i++) {
		mpz_init2(p->array[i], p->init_bit_size);
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
#if INSPECT_POOL
		p->push_count = p->small_count = 0;
		mpz_clear(p->push_sum);
#endif
	}
}

void pool_inspect(mpz_pool *p) {
#if INSPECT_POOL
	mpz_t avg;
	mpz_init(avg);
	mpz_cdiv_q_ui(avg, p->push_sum, p->push_count);
	printf("\npool: %lx, size: %zu, used: %zu, max used: %zu, avg: %zu bit\n%zu of %zu pushed integers are bigger then the init size of %zu bit.\n", (intptr_t)p, p->size, p->used, p->max_used, mpz_sizeinbase(avg, 2), p->small_count, p->push_count, p->init_bit_size);
	mpz_clear(avg);
#else
	printf("\npool: %lx, size: %zu, used: %zu\n", (intptr_t)p, p->size, p->used);
#endif
}

void pool_pop(mpz_pool *p, mpz_t ret) {
	size_t i;
	if (p->used >= p->size) {
#if INSPECT_POOL
		fprintf(stderr, "Pool %lx to small!\n", (intptr_t)p);
#endif
		p->array = (mpz_t *)realloc(p->array, (p->size + POOL_REALLOC_SIZE) * sizeof(mpz_t));
		for (i=p->size; i<(p->size + POOL_REALLOC_SIZE); i++) {
			mpz_init2(p->array[i], p->init_bit_size);
		}
		p->size += POOL_REALLOC_SIZE;
	}
	mpz_swap(p->array[p->used++], ret);
#if INSPECT_POOL
	if (p->used > p->max_used)
		p->max_used = p->used;
#endif
}

void pool_push(mpz_pool *p, mpz_t i) {
	if (p->used <= 0) {
		fprintf(stderr, "Can not push integer pool %lx is full!\n", (intptr_t)p);
	} else {
#if INSPECT_POOL
		size_t nbits;
		p->push_count++;
		nbits = mpz_sizeinbase(i, 2);
		if (nbits > p->init_bit_size)
			p->small_count++;
		mpz_add(p->push_sum, p->push_sum, i);
#endif
		mpz_swap(p->array[--p->used], i);
	}
}
