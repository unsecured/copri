// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `divide_conquer` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include "test.h"
#include "copri.h"
#include "divide_conquer.h"

int tests_passed = 0;
int tests_failed = 0;

void test_merge_mul_func(mpz_pool *pool, mpz_t ret, const mpz_t a, const mpz_t b) {
	mpz_mul(ret, a, b);
}

// Test divide_conquer by factorial.
static char * test_merge_mul(size_t n) {
	mpz_array a;
	mpz_t p, x;
	size_t g;
	mpz_pool pool;
	clock_t begin_divide_conquer, end_divide_conquer, end_array_prod, iterative_clock, recursive_clock;

	mpz_init(p);
	mpz_init(x);
	array_init(&a, 2);
	pool_init(&pool, 100);

	for(g=1;g<1+n;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	begin_divide_conquer = clock();
	array_divide_conquer(&pool, x, &a, &test_merge_mul_func);
	end_divide_conquer = clock();
	array_prod(&pool, &a, p);
	end_array_prod = clock();

	if (mpz_cmp(p, x) != 0) {
		return "ret value is wrong";
	}

	iterative_clock = end_divide_conquer - begin_divide_conquer;
	recursive_clock = end_array_prod - end_divide_conquer;

	if (iterative_clock > recursive_clock) {
		printf("WARN: slower (%ju) ", (uintmax_t)(iterative_clock-recursive_clock));
	}

	array_clear(&a);
	mpz_clear(p);
	mpz_clear(x);
	pool_clear(&pool);

	return 0;
}

// Execute all tests.
int main(int argc, char **argv) {

	printf("Starting divide_conquer test\n");

	printf("Testing mul 2                  ");
	test_evaluate(test_merge_mul(2));

	printf("Testing mul 3                  ");
	test_evaluate(test_merge_mul(3));

	printf("Testing mul 11                 ");
	test_evaluate(test_merge_mul(11));

	printf("Testing mul 130                ");
	test_evaluate(test_merge_mul(130));

	test_end();
}
