// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `stack` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "stack.h"

int tests_passed = 0;
int tests_failed = 0;

static char * test_stack() {
	mpz_stack *s;
	mpz_t a, b;
	size_t g;

	mpz_init(a);
	mpz_init(b);
	mpz_stack_init(&s);

	for(g=1;g<10;g++) {
		mpz_set_ui(a, g);
		mpz_stack_push(s, a);
	}

	mpz_stack_top(s, b);
	if (mpz_cmp(a, b) != 0) {
		return "top value differ!";
	}

	for(g=9;g>5;g--) {
		mpz_set_ui(a, g);
		mpz_stack_pop(s, b);
		if (mpz_cmp(a, b) != 0) {
			return "pop value differ!";
		}
	}

	mpz_stack_clear(s);
	mpz_clear(a);
	mpz_clear(b);

	return 0;
}

static char * test_stack_pair() {
	mpz_ui_pair_stack *s;
	mpz_t a, b;
	size_t g, x;

	mpz_init(a);
	mpz_init(b);
	mpz_ui_pair_stack_init(&s);

	for(g=1;g<10;g++) {
		mpz_set_ui(a, g*2);
		mpz_ui_pair_stack_push(s, g, a);
	}

	mpz_ui_pair_stack_top(s, &x, b);
	if (mpz_cmp(a, b) != 0) {
		return "top (mpz_t b) value differ!";
	} else if (9 != x) {
		return "top (size_t a) value differ!";
	}

	for(g=9;g>5;g--) {
		mpz_set_ui(a, g*2);
		mpz_ui_pair_stack_pop(s, &x, b);
		if (mpz_cmp(a, b) != 0 ||  g != x) {
			return "pop value differ!";
		}
	}

	mpz_ui_pair_stack_clear(s);
	mpz_clear(a);
	mpz_clear(b);

	return 0;
}

// Execute all tests.
int main(int argc, char **argv) {

	printf("Starting stack test\n");

	printf("Testing simple                 ");
	test_evaluate(test_stack());

	printf("Testing pair                   ");
	test_evaluate(test_stack_pair());

	test_end();
}
