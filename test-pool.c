// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `pool` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "pool.h"

int tests_passed = 0;
int tests_failed = 0;

int print_alloc = 0;

void *test_alloc(size_t size) {
	if (print_alloc)
		printf("alloc of %zu bytes\n", size);
	return malloc(size);
}


void setup() {
	mp_set_memory_functions(test_alloc, NULL, NULL);
}

static char * test_1() {
	mpz_pool p;
	mpz_t a, b;
	size_t i;
	print_alloc = 1;
	pool_init(&p, 1);

	for (i=0; i<12; i++) {
		pool_pop(&p, a);
		pool_pop(&p, b);
		mpz_add_ui(a, a, 1);
		mpz_add(b, a, b);
		print_alloc = 0;
		gmp_printf("%zu: %Zu\n", i, b);
		print_alloc = 1;
		pool_push(&p, b);
		pool_push(&p, a);
	}

	pool_clear(&p);

	return 0;
}

// Execute all tests.
int main(int argc, char **argv) {

	printf("Starting pool test\n");

	setup();

	printf("Testing                        ");
	test_evaluate(test_1());

	test_end();
}
