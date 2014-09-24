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

	mpz_t a;
	mpz_t b;
	mpz_array array1;
	mpz_array array2;

	array_init(&array1, 3);
	array_init(&array2, 3);

	// primes: 139, 223, 317, 577, 727, 863
	// `a = 139 * 223 * 317 * 577 = 5669630273`
	// `b = 317 * 577 * 727 * 863 = 114757289509`
	mpz_init_set_str(a, "5669630273", 10);
	mpz_init_set_str(b, "114757289509", 10);

	printf("\n\nappend_cb(a, b)\n");
	append_cb(&array1, a, b);
	array_print(&array1);
	printf("\n");

	printf("\n\nappend_cb(b, a)\n");
	append_cb(&array2, b, a);
	array_print(&array2);
	printf("\n");

	// `30997 = 139 * 223`
	// `627401 = 727 * 863`
	// `182909 = 317 * 577`

	mpz_clear(a);
	mpz_clear(b);
	array_clear(&array1);
	array_clear(&array2);

	return 0;
}

// **TODO**: Add an test. This function shows only the usage of `append_cb`.
static char * test_single() {

	mpz_t a;
	mpz_t b;
	mpz_array array1;
	mpz_array array2;

	array_init(&array1, 3);
	array_init(&array2, 3);

	// primes: 577, 727, 863
	// `a = 727 * 577 = 419479`
	// `b = 727 * 863 = 627401`
	mpz_init_set_str(a, "419479", 10);
	mpz_init_set_str(b, "627401", 10);

	printf("\n\nappend_cb(a, b)\n");
	append_cb(&array1, a, b);
	if (mpz_cmp_ui(a, 419479) != 0) {
		return "append_cb has changed the input value a!";
	}
	if (mpz_cmp_ui(b, 627401) != 0) {
		return "append_cb has changed the input value b!";
	}
	array_print(&array1);
	printf("\n");

	printf("\n\nappend_cb(b, a)\n");
	append_cb(&array2, b, a);
	if (mpz_cmp_ui(a, 419479) != 0) {
		return "append_cb has changed the input value a!";
	}
	if (mpz_cmp_ui(b, 627401) != 0) {
		return "append_cb has changed the input value b!";
	}
	array_print(&array2);
	printf("\n");

	mpz_clear(a);
	mpz_clear(b);
	array_clear(&array1);
	array_clear(&array2);

	return 0;
}

// Run all tests.
int main(int argc, char **argv) {
	
	printf("Starting append_cb test\n");
	
	printf("Testing                        ");
	test_evaluate(test_multiple());
	test_evaluate(test_single());

	test_end();
}
