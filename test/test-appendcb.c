// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `append_cb` function.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// **TODO**: Add an test. This function shows only the usage of `append_cb`.
static char * test_multiple() {

	mpz_t a, b;
	mpz_array array1, array2, array_expect;
	mpz_pool pool;

	pool_init(&pool, 0);
	array_init(&array1, 3);
	array_init(&array2, 3);
	array_init(&array_expect, 3);

	// `30997 = 139 * 223`
	// `627401 = 727 * 863`
	// `182909 = 317 * 577`
	mpz_init_set_str(a, "30997", 10);
	array_add(&array_expect, a);
	mpz_set_str(a, "182909", 10);
	array_add(&array_expect, a);
	mpz_set_str(a, "627401", 10);
	array_add(&array_expect, a);

	// primes: 139, 223, 317, 577, 727, 863
	// `a = 139 * 223 * 317 * 577 = 5669630273`
	// `b = 317 * 577 * 727 * 863 = 114757289509`
	mpz_set_str(a, "5669630273", 10);
	mpz_init_set_str(b, "114757289509", 10);

	append_cb(&pool, &array1, a, b);
	array_msort(&array1);
	if (!array_equal(&array_expect, &array1)) {
		return "array1 and array_expect differ!";
	}

	append_cb(&pool, &array2, b, a);
	array_msort(&array2);
	if (!array_equal(&array_expect, &array2)) {
		return "array2 and array_expect differ!";
	}

	mpz_clear(a);
	mpz_clear(b);
	array_clear(&array1);
	array_clear(&array2);
	array_clear(&array_expect);
	pool_clear(&pool);

	return 0;
}

// **TODO**: Add an test. This function shows only the usage of `append_cb`.
static char * test_single() {

	mpz_t a, b;
	mpz_array array1, array2, array_expect;
	mpz_pool pool;

	pool_init(&pool, 0);
	array_init(&array1, 3);
	array_init(&array2, 3);
	array_init(&array_expect, 3);

	// primes: 577, 727, 863
	mpz_init_set_str(a, "577", 10);
	array_add(&array_expect, a);
	mpz_set_str(a, "727", 10);
	array_add(&array_expect, a);
	mpz_set_str(a, "863", 10);
	array_add(&array_expect, a);

	// `a = 727 * 577 = 419479`
	// `b = 727 * 863 = 627401`
	mpz_set_str(a, "419479", 10);
	mpz_init_set_str(b, "627401", 10);

	append_cb(&pool, &array1, a, b);
	if (mpz_cmp_ui(a, 419479) != 0) {
		return "append_cb has changed the input value a!";
	}
	if (mpz_cmp_ui(b, 627401) != 0) {
		return "append_cb has changed the input value b!";
	}

	array_msort(&array1);
	if (!array_equal(&array_expect, &array1)) {
		return "array1 and array_expect differ!";
	}

	append_cb(&pool, &array2, b, a);
	if (mpz_cmp_ui(a, 419479) != 0) {
		return "append_cb has changed the input value a!";
	}
	if (mpz_cmp_ui(b, 627401) != 0) {
		return "append_cb has changed the input value b!";
	}

	array_msort(&array2);
	if (!array_equal(&array_expect, &array2)) {
		return "array2 and array_expect differ!";
	}

	mpz_clear(a);
	mpz_clear(b);
	array_clear(&array1);
	array_clear(&array2);
	array_clear(&array_expect);
	pool_clear(&pool);

	return 0;
}

// Run all tests.
int main(int argc, char **argv) {
	
	printf("Starting append_cb test\n");
	
	printf("Testing single                 ");
	test_evaluate(test_single());
	printf("Testing multiple               ");
	test_evaluate(test_multiple());

	test_end();
}
