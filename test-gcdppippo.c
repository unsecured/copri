// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `gcd_ppi_ppo` function.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// This function calculates `ppi`, `ppo` and `gcd` and compares the value to the expected strings.
static char * test_gcd_ppi_ppo(char * str_a, char * str_b, char * str_expect_gcd, char * str_expect_ppi, char * str_expect_ppo) {
	mpz_t a;
	mpz_t b;
	mpz_t a0;
	mpz_t b0;
	mpz_t gcd;
	mpz_t ppi;
	mpz_t ppo;

	mpz_t expect_gcd;
	mpz_t expect_ppi;
	mpz_t expect_ppo;

	mpz_pool pool;

	pool_init(&pool, 0);
	mpz_init_set_str(a, str_a, 10);
	mpz_init_set_str(b, str_b, 10);
	mpz_init_set(a0, a);
	mpz_init_set(b0, b);

	mpz_init(gcd);
	mpz_init(ppi);
	mpz_init(ppo);

	mpz_init_set_str(expect_gcd, str_expect_gcd, 10);
	mpz_init_set_str(expect_ppi, str_expect_ppi, 10);
	mpz_init_set_str(expect_ppo, str_expect_ppo, 10);

	gcd_ppi_ppo(&pool, gcd, ppi, ppo, a, b);

	// Check the return values.
	if (mpz_cmp(expect_gcd, gcd) != 0) {
		return "gcd calculation error";
	}
	if (mpz_cmp(expect_ppi, ppi) != 0) {
		return "ppi calculation error";
	}
	if (mpz_cmp(expect_ppo, ppo) != 0) {
		return "ppo calculation error";
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
	mpz_clear(ppi);
	mpz_clear(ppo);
	mpz_clear(expect_gcd);
	mpz_clear(expect_ppi);
	mpz_clear(expect_ppo);
	pool_clear(&pool);

	return 0;
}

// Run all tests.
int main(int argc, char **argv) {

	printf("Starting gcd_ppi_ppo test\n");

	printf("Test 1                         ");
	//     a = 3^2 * 7^3 * 13 = 40131
	//     b = 2^5 * 3 * 7^2 * 11 = 51744
	//     gcd(a,b) = 147
	//     ppi = 3^2 * 7^3 = 3087
	//     ppo = 3^2 * 7^3 * 13 = 13
	test_evaluate(test_gcd_ppi_ppo("40131", "51744", "147", "3087", "13"));

	test_end();
}
