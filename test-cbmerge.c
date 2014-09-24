// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `cbmerge` function.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// **Test `test`**.
static char * test() {

	mpz_array in1;
	mpz_array in2;
	mpz_array out;
	mpz_t b;
	mpz_pool pool;

	pool_init(&pool, 0);
	array_init(&in1, 10);
	array_init(&in2, 10);
	array_init(&out, 3);

	// primes: 577, 727, 863
	// `a = 727 * 577 = 419479`
	// `b = 727 * 863 = 627401`
	mpz_init_set_str(b, "419479", 0);
	array_add(&in1, b);

	mpz_set_str(b, "627401", 0);
	array_add(&in1, b);

	printf("\nin1:\n");
	array_print(&in1);

	// primes: 317, 577, 223, 863
	// `a = 317 * 577 = 182909`
	// `b = 223 * 863 = 192449`
	mpz_set_str(b, "182909", 0);
	array_add(&in2, b);

	mpz_set_str(b, "192449", 0);
	array_add(&in2, b);

	printf("\nin2:\n");
	array_print(&in2);

	cbmerge(&pool, &out, &in1, &in2);

	printf("\nout:\n");
	array_print(&out);

	array_clear(&in1);
	array_clear(&in2);
	array_clear(&out);
	mpz_clear(b);
	pool_clear(&pool);

	return 0;
}


// Run all tests.
int main(int argc, char **argv) {

	printf("Starting cbmerge test\n");

	printf("Test1                          ");
	test_evaluate(test());

	test_end();
}
