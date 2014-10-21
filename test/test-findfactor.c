// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `find_factor` functions.
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

	// primes: 139, 223, 317, 577, 727, 4513
	mpz_init_set_str(b, "139", 0);
	array_add(&in, b);

	mpz_set_str(b, "223", 0);
	array_add(&in, b);

	mpz_set_str(b, "577", 0);
	array_add(&in, b);

	mpz_set_str(b, "230459", 0); // 727 * 317
	array_add(&in, b);

	mpz_set_str(b, "627401", 0); // 139 * 4513
	array_add(&array_expect, b);

	mpz_set_str(b, "139", 0);
	array_add(&array_expect, b);

	mpz_set_str(b, "4513", 0);
	array_add(&array_expect, b);

	mpz_set_str(b, "627401", 0);
	array_find_factor(&pool, &out, b, &in);

	if (!array_equal(&array_expect, &out)) {
		return "out and array_expect differ!";
	}

	array_clear(&out);
	array_clear(&in);
	array_clear(&array_expect);
	mpz_clear(b);
	pool_clear(&pool);

	return 0;
}


// Run all tests.
int main(int argc, char **argv) {

	printf("Starting find_factor test\n");

	printf("Test1                          ");
	test_evaluate(test());

	test_end();
}
