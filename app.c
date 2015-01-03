// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This file contains a simple application of the
// [copri](copri.html) library.
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include "copri.h"
#include "config.h"

// Start by defining an neat looking banner.
#define PRINT_BANNER printf(""\
"   _______  _______  _______  _______ _________    \n"\
"  (  ____ \\(  ___  )(  ____ )(  ____ )\\__   __/    \n"\
"  | (    \\/| (   ) || (    )|| (    )|   ) (       \n"\
"  | |      | |   | || (____)|+ (____)|   | |       \n"\
"  | |      | |   | ||  _____)|     __)   | |       \n"\
"  | |      | |   | || (      | (\\ (      | |       \n"\
"  | (____/\\| (___) |+ )      | ) \\ \\_____) (___    \n"\
"  (_______/(_______)|/       |/   \\__/\\_______/    \n"\
"                                                   \n"\
"    Implementation of 'Factoring into coprimes     \n"\
"           in essentially linear time'             \n"\
"                                                   \n"\
"      by Martin Wind and Gerhard Reithofer         \n"\
"                                                   \n"\
"        Algorithm by Daniel J. Bernstein           \n"\
"   http://cr.yp.to/lineartime/dcba-20040404.pdf    \n\n");

// The generic `main` function.
//
// Define all variables at the beginning to make the C99 compiler
// happy.
int main(int argc, char **argv) {
	mpz_array s, p, out;
	mpz_pool pool;
	size_t count, i;
	int c, vflg = 0, sflg = 0, rflg = 0, errflg = 0, r = 0;
	char *filename = "primes.lst";
	char *cb_file = NULL;

	// #### argument parsing
	// Boring `getopt` argument parsing.
	while ((c = getopt(argc, argv, ":svrb:")) != -1) {
		switch(c) {
		case 'b':
			cb_file = optarg;
			break;
		case 's':
			sflg++;
			break;
		case 'v':
			vflg++;
			break;
		case 'r':
			rflg++;
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

	if (rflg && vflg) {
		fprintf(stderr, "\n\t-r and -v can't be used simultaneously!\n\n");
		errflg++;
	}

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg) {
		fprintf(stderr, "usage: [-vsr] [file]\n"\
                        "\n\t-b FILE   store the coprime base in FILE"\
                        "\n\t-v        be more verbose"\
                        "\n\t-r        output the found coprimes in raw gmp format"\
                        "\n\t-s        only check if there are coprimes"\
                        "\n\n");
		exit(2);
	}

	// Print the banner.
	if (vflg > 0) {
		PRINT_BANNER;
#if INSPECT_POOL
		printf("WARNING: This build uses the `inspect-pool` debug option!\n");
#endif
#if !(USE_OPENMP)
		printf("WARNING: This build does not use OpenMP multithreading!!!\n");
#endif
#if INSPECT_POOL || !(USE_OPENMP)
		printf("\n");
#endif
	}

	// Load the keys.
	array_init(&s, 10);
	count = array_of_file(&s, filename);
	pool_init(&pool, count);
	if (count == 0) {
		fprintf(stderr, "Can't load %s\n", filename);
		return 1;
	}
	if (s.used != count) {
		fprintf(stderr, "Array size and load count do not match\n");
		return 2;
	}
	if (s.used == 0) {
		fprintf(stderr, "No primes loaded (empty file)\n");
		return 3;
	}
	// Print the key count.
	if (vflg > 0) {
		printf("%zu public keys loaded\n", s.used);
		if (cb_file != NULL)
			printf("cb is going to be saved in '%s'\n", cb_file);
		printf("Starting factorization...\n");
	}

	// Computing a coprime base for a finite set [Algorithm 18.1](copri.html#computing-a-coprime-base-for-a-finite-set).
	array_init(&p, s.used);
	array_cb(&pool, &p, &s);

	if (cb_file != NULL) {
		if (vflg > 0) {
			printf("storing cb in '%s'\n", cb_file);
		}
		array_to_file(&p, cb_file);
	}


	// Check if we have found more coprime bases.
	if (p.used == s.used) {
		if (vflg > 0)
			printf("No coprime pairs found :-(\n");
		r = 1;
	} else {
		if (vflg > 0 || sflg > 0)
			printf("Found ~%zu coprime pairs!!!\nSearching factors...\n", (p.used - s.used));
		if (sflg == 0) {
			array_init(&out, 9);
			// Use [Algorithm 21.2](copri.html#factoring-a-set-over-a-coprime-base) to find the coprimes in the coprime base.
			array_find_factors(&pool, &out, &s, &p);

			// Output the factors.
			if (out.used > 0) {
				if ((out.used % 3) != 0) {
					fprintf(stderr, "Find factors returned an invalid array\n");
				} else {
					if (rflg) {
						for(i = 0; i < out.used; i++)
							mpz_out_raw(stdout, out.array[i]);
					} else {
						for(i = 0; i < out.used; i+=3)
							gmp_printf("\n### Found factors of\n%Zu\n=\n%Zu\nx\n%Zu\n", out.array[i], out.array[i+1], out.array[i+2]);
					}
				}
			}
			array_clear(&out);
		}
	}

	array_clear(&p);
	array_clear(&s);
	if (vflg > 0)
		pool_inspect(&pool);
	pool_clear(&pool);

	return r;
}
