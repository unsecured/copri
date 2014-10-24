/*
 * divide_conquer.h
 *
 *  Created on: 22.10.2014
 *      Author: mawi
 */

#ifndef DIVIDE_CONQUER_H_
#define DIVIDE_CONQUER_H_

#include <gmp.h>
#include "array.h"
#include "pool.h"

size_t array_divide_conquer(mpz_pool *pool, mpz_t ret, mpz_array *in, void(*merge)(mpz_pool *, mpz_t, const mpz_t, const mpz_t));

#endif /* DIVIDE_CONQUER_H_ */
