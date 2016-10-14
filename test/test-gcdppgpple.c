// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `gcd_ppg_pple` function.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// This function calculates `ppg`, `pple` and `gcd` and compares the value to the expected strings.
static char * test_gcd_ppg_pple(char * str_a, char * str_b, char * str_expect_gcd, char * str_expect_ppg, char * str_expect_pple) {
	mpz_t a;
	mpz_t b;
	mpz_t a0;
	mpz_t b0;
	mpz_t gcd;
	mpz_t ppg;
	mpz_t pple;

	mpz_t expect_gcd;
	mpz_t expect_ppg;
	mpz_t expect_pple;

	mpz_pool pool;

	pool_init(&pool, 0);
	mpz_init_set_str(a, str_a, 10);
	mpz_init_set_str(b, str_b, 10);
	mpz_init_set(a0, a);
	mpz_init_set(b0, b);

	mpz_init(gcd);
	mpz_init(ppg);
	mpz_init(pple);

	mpz_init_set_str(expect_gcd, str_expect_gcd, 10);
	mpz_init_set_str(expect_ppg, str_expect_ppg, 10);
	mpz_init_set_str(expect_pple, str_expect_pple, 10);

	gcd_ppg_pple(&pool, gcd, ppg, pple, a, b);

	// Check the return values.

	if (mpz_cmp(expect_gcd, gcd) != 0) {
		return "gcd calculation error";
	}

	if (mpz_cmp(expect_ppg, ppg) != 0) {
		return "ppg calculation error";
	}

	if (mpz_cmp(expect_pple, pple) != 0) {
		return "pple calculation error";
	}

	if (mpz_cmp(a, a0) != 0) {
		return "input value a has changed";
	}
	if (mpz_cmp(b, b0) != 0) {
		return "input value b has changed";
	}

	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(a0);
	mpz_clear(b0);
	mpz_clear(gcd);
	mpz_clear(ppg);
	mpz_clear(pple);
	mpz_clear(expect_gcd);
	mpz_clear(expect_ppg);
	mpz_clear(expect_pple);
	pool_clear(&pool);

	return 0;
}

// Run all tests.
int main(int argc, char **argv) {

	printf("Starting gcd_ppg_pple test\n");

	printf("Test 1                         ");
	//     a = 3^2 * 7^3 * 13 = 40131
	//     b = 2^5 * 3 * 7^3 * 11 = 362208
	//     gcd(a,b) = 1029
	//     ppg = 3^2 * 13 = 117
	//     pple = 7^3 = 343
	test_evaluate(test_gcd_ppg_pple("40131", "362208", "1029", "117", "343"));

	test_end();
}
