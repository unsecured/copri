// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This is a test of [copri](copri.html) `array_init`, `array_add` and `array_init` functions.
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "test.h"
#include "copri.h"

int tests_passed = 0;
int tests_failed = 0;

// Add 10 integers to an 4 sized array.
static char * test_add() {
	mpz_array a;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small

	for(g=0;g<10;g++) {
		array_add(&a, p);
	}

	array_clear(&a);
	mpz_clear(p);

	return 0;
}

// Add 10 integers and validate all values.
static char * test_values() {
	mpz_array a;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small

	for(g=0;g<10;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	for(g=0;g<a.used;g++) {
		if (mpz_get_ui(a.array[g]) != g)
			return "error";
	}

	array_clear(&a);
	mpz_clear(p);

	return 0;
}

// Creates an array and add 10 integers and validate all values.
// Finally free the memory of the array.
static char * test_clear() {
	mpz_array a;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small

	for(g=0;g<10;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	for(g=0;g<a.used;g++) {
		if (mpz_get_ui(a.array[g]) != g)
			return "error";
	}

	mpz_clear(p);
	array_clear(&a);

	return 0;
}

// Test merge sort.
static char * test_msort() {
	mpz_array a;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small

	for(g=0;g<10;g=g+2) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	for(g=1;g<10;g=g+2) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	array_msort(&a);

	for(g=0;g<a.used;g++) {
		if (mpz_get_ui(a.array[g]) != g) {
			printf("%zu != %zu at %zu\n", mpz_get_ui(a.array[g]), g, g);
			return "error";
		}

	}

	array_clear(&a);
	mpz_clear(p);

	return 0;
}

// Test merge sort.
static char * test_msort_2() {
	mpz_array a;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small


	for(g=10;g>0;g--) {
		mpz_set_ui(p, g-1);
		array_add(&a, p);
	}

	array_msort(&a);

	for(g=0;g<a.used;g++) {
		if (mpz_get_ui(a.array[g]) != g) {
			printf("%zu != %zu at %zu\n", mpz_get_ui(a.array[g]), g, g);
			return "error";
		}

	}

	array_clear(&a);
	mpz_clear(p);

	return 0;
}

// Creates an array and add 10 integers and validate all values.
// Finally free the memory of the array.
static char * test_copy() {
	mpz_array a, b;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small
	array_init(&b, 4); // to small

	for(g=0;g<10;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	array_copy(&b, &a);
	//array_print(&b);

	for(g=0;g<b.used;g++) {
		if (mpz_get_ui(b.array[g]) != g)
			return "error 1";
	}

	array_copy(&b, &a);
	//array_print(&b);

	for(g=0;g<b.used;g++) {
		if (mpz_get_ui(b.array[g]) != (g % 10))
			return "error 2";
	}

	mpz_set_ui(p, 7);
	array_add(&b, p);
	array_add(&b, p);

	array_copy(&b, &a);

	for(g=0;g<b.used;g++) {
		if (g < 20) {
			if (mpz_get_ui(b.array[g]) != (g % 10))
				return "error 3";
		} else if (g > 21) {
			if (mpz_get_ui(b.array[g]) != ((g-2) % 10))
				return "error 4";
		} else {
			if (mpz_get_ui(b.array[g]) != 7)
				return "error 5";
		}
	}


	mpz_clear(p);
	array_clear(&a);
	array_clear(&b);

	return 0;
}

// Test the array equal util.
static char * test_equal() {
	mpz_array a, b;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small
	array_init(&b, 4); // to small

	for(g=0;g<10;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
		array_add(&b, p);
	}

	if (!array_equal(&a, &b)) {
		return "array a and b differ!";
	}

	array_add(&b, p);

	if (array_equal(&a, &b)) {
		return "array a and b do not differ!";
	}

	array_clear(&a);
	array_clear(&b);
	mpz_clear(p);

	return 0;
}

// Test the array contains util.
static char * test_contains() {
	mpz_array a;
	mpz_t p;
	size_t g;

	mpz_init(p);
	array_init(&a, 4); // to small

	for(g=0;g<10;g++) {
		mpz_set_ui(p, g);
		array_add(&a, p);
	}

	mpz_set_ui(p, 7);

	if (!array_contains(&a, p)) {
		return "array contains wrong on containing integer";
	}

	mpz_set_ui(p, 12);

	if (array_contains(&a, p)) {
		return "array contains wrong on not containing integer";
	}

	array_clear(&a);
	mpz_clear(p);

	return 0;
}

// Test the array unique util.
static char * test_unique() {
	mpz_array sorted, uniques;
	mpz_t p;
	size_t g;
	//array_unique
	mpz_init(p);
	array_init(&sorted, 4); // to small
	array_init(&uniques, 4); // to small

	for(g=0;g<10;g++) {
		mpz_set_ui(p, g);
		array_add(&sorted, p);
		array_add(&sorted, p);
	}

	if (sorted.used != 20) {
		return "wrong sorted size";
	}

	array_unique(&uniques, &sorted);

	if (uniques.used != 10) {
		return "wrong uniques size";
	}

	for(g=0;g<10;g++) {
		if (mpz_get_ui(uniques.array[g]) != g) {
			printf("%zu != %zu at %zu\n", mpz_get_ui(uniques.array[g]), g, g);
			return "error";
		}
	}

	return 0;
}

// Execute all tests.
int main(int argc, char **argv) {
	
	printf("Starting array test\n");
	
	printf("Testing add                    ");
	test_evaluate(test_add());
	
	printf("Testing values                 ");
	test_evaluate(test_values());

	printf("Testing clear                  ");
	test_evaluate(test_clear());

	printf("Testing copy                   ");
	test_evaluate(test_copy());

	printf("Testing msort                  ");
	test_evaluate(test_msort());

	printf("Testing msort_2                ");
	test_evaluate(test_msort_2());

	printf("Testing equal                  ");
	test_evaluate(test_equal());

	printf("Testing contains               ");
	test_evaluate(test_contains());

	printf("Testing unique                 ");
	test_evaluate(test_unique());

	test_end();
}
