// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

#ifndef ARRAY_H
#define ARRAY_H

typedef struct {
	mpz_t * array;
	size_t used;
	size_t size;
} mpz_array;

void array_init(mpz_array *a, size_t size);

void array_add(mpz_array *a, const mpz_t integer);

void array_add_array(mpz_array *target, mpz_array *src);

void array_copy(mpz_array *target, mpz_array *src);

void array_clear(mpz_array *a);

int array_swap(mpz_array *a, mpz_array *b);

int array_equal(mpz_array *a, mpz_array *b);

void array_print(mpz_array *a);

size_t array_of_file(mpz_array *a, const char *filename);

size_t array_to_file(mpz_array *a, const char *filename);

void array_msort(mpz_array *a);

int array_contains(mpz_array *a, const mpz_t integer);

void array_unique(mpz_array *uniques, mpz_array *sorted);

#endif /* ARRAY_H */
