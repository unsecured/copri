// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This app implements a n^2 test application.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include "copri.h"

int main(int argc, char **argv) {
	mpz_array s;
	mpz_t g;
	size_t count, i, j;

	int c, vflg = 0, errflg = 0;
	char *filename = "primes.lst";

	while ((c = getopt(argc, argv, ":sv")) != -1) {
		switch(c) {
		case 'v':
			vflg++;
			break;
		case ':':
			fprintf(stderr, "Option -%c requires an operand\n", optopt);
			errflg++;
			break;
		case '?':
			fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
			errflg++;
		}
	}

	if (optind < argc) {
		filename = argv[optind];
		if (optind + 1 < argc) errflg++;
	}

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg) {
		fprintf(stderr, "usage: [-v] [file]\n"\
						"\n\t-v        be more verbose"\
						"\n\n");
		exit(2);
	}

	array_init(&s, 10);
	mpz_init(g);

	count = array_of_file(&s, filename);
	if (count == 0) {
		fprintf(stderr, "Can't load %s", filename);
		return 1;
	}
	if (s.used != count) {
		fprintf(stderr, "Array size and load count do not match");
		return 2;
	}
	if (s.used == 0) {
		fprintf(stderr, "No primes loaded (empty file)");
		return 3;
	}
	if (vflg > 0) {
		printf("Loaded %zu primes\nStarting factorization...\n", s.used);
	}

	for(i = 0; i < s.used; i++) {
		for(j = 0; j < s.used; j++) {
			if (j > i) {
				mpz_gcd(g, s.array[i], s.array[j]);
				if (mpz_cmp_ui(g, 1) != 0) {
					gmp_printf("Found coprime ----\n%Zu\nand\n%Zu\nshare\n%Zu\n----\n", s.array[i], s.array[j], g);
				}
			}
		}
	}

	array_clear(&s);
	mpz_clear(g);

	return 0;
}
