// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#ifndef COPRI_H
#define COPRI_H

#include "array.h"
#include "pool.h"

void two_power(mpz_t rot, unsigned long long n);

void gcd_ppi_ppo(mpz_pool *pool, mpz_t gcd, mpz_t ppi, mpz_t ppo, const mpz_t a, const mpz_t c);

void ppi_ppo(mpz_pool *pool, mpz_t ppi, mpz_t ppo, const mpz_t a, const mpz_t c);

void ppi(mpz_pool *pool, mpz_t ppi, const mpz_t a, const mpz_t c);

void gcd_ppg_pple(mpz_pool *pool, mpz_t gcd, mpz_t ppg, mpz_t pple, const mpz_t a, const mpz_t b);

void append_cb(mpz_pool *pool, mpz_array *out, const mpz_t a, const mpz_t b);

void prod(mpz_pool *pool, mpz_ptr rot, mpz_t * array, size_t from, size_t to);

void array_prod(mpz_pool *pool, mpz_array *a, mpz_ptr rot);

void split(mpz_pool *pool, mpz_array *ret, const mpz_t a, mpz_t *p, size_t from, size_t to);

void array_split(mpz_pool *pool, mpz_array *ret, const mpz_t a, mpz_array *p);

void cbextend(mpz_pool *pool, mpz_array *ret, mpz_array *p, const mpz_t b);

void cbmerge(mpz_pool *pool, mpz_array *s, mpz_array *p, mpz_array *q);

void cb(mpz_pool *pool, mpz_array *ret, mpz_t *s, size_t from, size_t to);

void array_cb(mpz_pool *pool, mpz_array *ret, mpz_array *s);

void reduce(mpz_pool *pool, mpz_t i, mpz_t pai, const mpz_t p, const mpz_t a);

int find_factor(mpz_pool *pool, mpz_array *out, const mpz_t a0, const mpz_t a, mpz_t *p, size_t from, size_t to);

int array_find_factor(mpz_pool *pool, mpz_array *out, const mpz_t a, mpz_array *p);

void find_factors(mpz_pool *pool, mpz_array *out, mpz_t *s, size_t from, size_t to, mpz_array *p);

void array_find_factors(mpz_pool *pool, mpz_array *out, mpz_array *s, mpz_array *p);

#endif /* COPRI_H */
