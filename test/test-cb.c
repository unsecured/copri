// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `cb` and `array_cb` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// **Test `test`**.
static char * test() {

	mpz_array in, out, array_expect;
	mpz_t b;
	mpz_pool pool;

	pool_init(&pool, 0);
	array_init(&in, 10);
	array_init(&out, 3);
	array_init(&array_expect, 3);

	// primes: 577, 727, 863
	mpz_init_set_str(b, "577", 10);
	array_add(&array_expect, b);
	mpz_set_str(b, "727", 10);
	array_add(&array_expect, b);
	mpz_set_str(b, "863", 10);
	array_add(&array_expect, b);

	// `727 * 577 = 419479`
	// `863 * 577 = 497951`
	// `727 * 863 = 627401`
	mpz_set_str(b, "419479", 0);
	array_add(&in, b);
	mpz_set_str(b, "627401", 0);
	array_add(&in, b);
	mpz_set_str(b, "497951", 0);
	array_add(&in, b);

	array_cb(&pool, &out, &in);

	array_msort(&out);
	if (!array_equal(&array_expect, &out)) {
		return "out and array_expect differ!";
	}

	mpz_clear(b);
	array_clear(&in);
	array_clear(&out);
	array_clear(&array_expect);
	pool_clear(&pool);

	return 0;
}


// Run all tests.
int main(int argc, char **argv) {

	printf("Starting cb test\n");

	printf("Test1                          ");
	test_evaluate(test());

	test_end();
}
