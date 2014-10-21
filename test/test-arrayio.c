// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `array_of_file` and `array_to_file` functions.
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;


void add_test_data(mpz_array *a) {
	mpz_t b;
	mpz_init_set_str(b, "3423423", 10);
	array_add(a, b);
	mpz_set_str(b, "938474857283", 10);
	array_add(a, b);
	mpz_set_str(b, "456456456", 10);
	array_add(a, b);
	mpz_set_str(b, "73522342342342", 10);
	array_add(a, b);
	mpz_set_str(b, "1354564564353", 10);
	array_add(a, b);
	mpz_set_str(b, "523564755654", 10);
	array_add(a, b);
	mpz_set_str(b, "8453453534537", 10);
	array_add(a, b);
}

// Loads an array from an file.
static char * test_to_file() {
	mpz_array a;
	size_t count;
	array_init(&a, 8);
	add_test_data(&a);
	unlink("test/test.lst");
	count = array_to_file(&a, "test/test.lst");
	if (count == 0) return "Can't save to test/test.lst";
	if (a.used != count) return "Array size and write count do not match";
	return 0;
}

static char * test_of_file() {
	mpz_array a, b;
	size_t count, i;
	array_init(&a, 8);
	array_init(&b, 8);
	add_test_data(&b);
	count = array_of_file(&a, "test/test.lst");
	if (count == 0) return "Can't read test/test.lst";
	if (a.used != count) return "Array size and read count do not match";
	if (a.used != b.used) return "Array size is not equal";
	for(i = 0; i < a.used; i++) {
		if (mpz_cmp(a.array[i], b.array[i]) != 0)
			return "test/test.lst dose not contain the test data";
	}
	return 0;
}


// Execute all tests.
int main(int argc, char** argv) {
	
	printf("Starting array io test\n");
	
	printf("Testing array_to_file          ");
	test_evaluate(test_to_file());

	printf("Testing array_of_file          ");
	test_evaluate(test_of_file());
	
	test_end();
}
