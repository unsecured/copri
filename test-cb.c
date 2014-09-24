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

	mpz_array in;
	mpz_array out;
	mpz_t b;

	array_init(&in, 10);
	array_init(&out, 3);

	mpz_init_set_str(b, "419479", 0);
	array_add(&in, b);

	mpz_set_str(b, "627401", 0);
	array_add(&in, b);

	mpz_set_str(b, "497951", 0);
	array_add(&in, b);

	printf("\nin1:\n");
	array_print(&in);

	array_cb(&out, &in);

	printf("\nout:\n");
	array_print(&out);

	mpz_clear(b);
	array_clear(&in);
	array_clear(&out);

	return 0;
}


// Run all tests.
int main(int argc, char **argv) {

	printf("Starting cb test\n");

	printf("Test1                          ");
	test_evaluate(test());

	test_end();
}
