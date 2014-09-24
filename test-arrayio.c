// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `array_of_file` and `array_to_file` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// Loads an array from an file.
static char * test_load_gen() {
	mpz_array a;
	size_t count;
	array_init(&a, 100);
	count = array_of_file(&a, "primes.lst");
	if (count == 0) return "Can't load primes.lst ! run ./gen first !";
	if (a.used != count) return "Array size and load count do not match";
	/* for(i = 0; i < a.used; i++) printf("%s\n", mpz_get_str(NULL, 10, a.array[i])); */
	return 0;
}


// Execute all tests.
int main(int argc, char** argv) {
	
	printf("Starting array io test\n");
	
	printf("Testing load from ./gen util   ");
	test_evaluate(test_load_gen());
	
	test_end();
}
