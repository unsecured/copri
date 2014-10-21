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

	mpz_array in1, in2, out, array_expect;
	mpz_t b;
	mpz_pool pool;

	pool_init(&pool, 0);
	array_init(&in1, 2);
	array_init(&in2, 2);
	array_init(&out, 5);
	array_init(&array_expect, 5);

	mpz_init_set_str(b, "317", 10);
	array_add(&array_expect, b);
	mpz_set_str(b, "577", 10);
	array_add(&array_expect, b);
	mpz_set_str(b, "727", 10);
	array_add(&array_expect, b);
	mpz_set_str(b, "196463", 10);
	array_add(&array_expect, b);
	mpz_set_str(b, "346063", 10);
	array_add(&array_expect, b);

	// primes: 401, [577], 727, 863
	// `a = 727 * 577 = 419479`
	// `b = 401 * 863 = 346063`
	mpz_set_str(b, "419479", 10);
	array_add(&in1, b);

	mpz_set_str(b, "346063", 10);
	array_add(&in1, b);

	// primes: 317, 223, [577], 881
	// `a = 317 * 577 = 182909`
	// `b = 223 * 881 = 196463`
	mpz_set_str(b, "182909", 10);
	array_add(&in2, b);

	mpz_set_str(b, "196463", 10);
	array_add(&in2, b);

	cbmerge(&pool, &out, &in1, &in2);

	array_msort(&out);
	if (!array_equal(&array_expect, &out)) {
		return "out and array_expect differ!";
	}

	array_clear(&in1);
	array_clear(&in2);
	array_clear(&out);
	array_clear(&array_expect);
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
