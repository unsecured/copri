// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#ifndef POOL_H
#define POOL_H

#include "array.h"

typedef struct {
	mpz_t *array;
	size_t used;
	size_t size;
} mpz_pool;

void pool_init(mpz_pool *p, size_t size);

void pool_clear(mpz_pool *p);

void pool_pop(mpz_pool *p, mpz_t ret);

void pool_push(mpz_pool *p, mpz_t i);

#endif /* POOL_H */
