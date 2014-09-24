// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `split` and `array_split` functions.
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
	mpz_pool pool;

	pool_init(&pool, 0);
	array_init(&in, 10);
	array_init(&out, 3);

	// primes: 139, 223, 317, 577, 727, 863
	mpz_init_set_str(b, "139", 0);
	array_add(&in, b);

	mpz_set_str(b, "223", 0);
	array_add(&in, b);

	mpz_set_str(b, "317", 0);
	array_add(&in, b);

	mpz_set_str(b, "577", 0);
	array_add(&in, b);

	mpz_set_str(b, "727", 0);
	array_add(&in, b);

	mpz_set_str(b, "863", 0);
	array_add(&in, b);

	printf("\nin:\n");
	array_print(&in);

	printf("v: 627401\n"); // 727 * 863

	mpz_set_str(b, "627401", 0);

	array_split(&pool, &out, b, &in);

	printf("\nout:\n");
	array_print(&out);

	array_clear(&in);
	array_clear(&out);
	mpz_clear(b);
	pool_clear(&pool);

	return 0;
}


// Run all tests.
int main(int argc, char **argv) {

	printf("Starting split test\n");

	printf("Test1                          ");
	test_evaluate(test());

	test_end();
}
