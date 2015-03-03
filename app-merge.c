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

// The generic `main` function.
//
// Define all variables at the beginning to make the C99 compiler
// happy.
int main(int argc, char **argv) {
	mpz_array s1, s2, p, out;
	mpz_pool pool;
	size_t c1, c2, i;
	int c, vflg = 0, sflg = 0, rflg = 0, jflg = 0, errflg = 0, r = 0;
	char *file1 = "primes1.lst";
	char *file2 = "primes2.lst";
	char *cb_file = NULL;

	// #### argument parsing
	// Boring `getopt` argument parsing.
	while ((c = getopt(argc, argv, ":svrjb:")) != -1) {
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
		case 'j':
			jflg++;
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
		file1 = argv[optind];
		file2 = argv[optind+1];
		if (optind + 2 < argc) errflg++;
	}

	if (rflg && vflg) {
		fprintf(stderr, "\n\t-r and -v can't be used simultaneously!\n\n");
		errflg++;
	}

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg) {
		fprintf(stderr, "usage: [-vsr] [-b out-file] [cb-file1] [cb-file1]\n"\
                        "\n\t-b FILE   store the coprime base in FILE"\
                        "\n\t-v        be more verbose"\
						"\n\t-j        use json as output format"\
                        "\n\t-r        output the found coprimes in raw gmp format"\
                        "\n\t-s        only check if there are coprimes"\
                        "\n\n");
		exit(2);
	}

	// Print the banner.
	if (vflg > 0) {
#if INSPECT_POOL
		printf("WARNING: This build uses the `inspect-pool` debug option!\n\n");
#endif
	}

	// Load the keys.
	array_init(&s1, 10);
	c1 = array_of_file(&s1, file1);
	array_init(&s2, 10);
	c2 = array_of_file(&s2, file2);

	pool_init(&pool, c1);
	if (c1 == 0) {
		fprintf(stderr, "Can't load %s\n", file1);
		return 1;
	}
	if (c2 == 0) {
		fprintf(stderr, "Can't load %s\n", file2);
		return 1;
	}
	if (s1.used != c1 ||  s2.used != c2) {
		fprintf(stderr, "Array size and load count do not match\n");
		return 2;
	}
	// Print the key count.
	if (vflg > 0 && jflg == 0) {
		printf("coprime base 1 size: %zu\ncoprime base 2 size: %zu\n", s1.used, s2.used);
		if (cb_file != NULL)
			printf("cb is going to be saved in '%s'\n", cb_file);
		printf("Starting factorization...\n");
	} else if (jflg > 0) {
		printf("{\"type\":\"start\",\"msg\":\"Starting factorization\",\"count\":[%zu,%zu]}\n", s1.used, s2.used);
		fflush(stdout);
	}

	// Computing a coprime base for a finite set [Algorithm 18.1](copri.html#computing-a-coprime-base-for-a-finite-set).
	array_init(&p, s1.used+s2.used);
	cbmerge(&pool, &p, &s1, &s2);

	if (cb_file != NULL) {
		if (vflg > 0) {
			if (jflg == 0) {
				printf("storing cb in '%s'\n", cb_file);
			} else {
				printf("{\"type\":\"store\",\"msg\":\"Storing coprimebase\",\"file\":\"%s\"}\n", cb_file);
				fflush(stdout);
			}
		}
		array_to_file(&p, cb_file);
	}


	// Check if we have found more coprime bases.
	if (p.used == s1.used + s2.used) {
		if (vflg > 0) {
			if (jflg == 0) {
				printf("No coprime pairs found :-(\n");
			} else {
				printf("{\"type\":\"info\",\"msg\":\"No coprime pairs found\"}\n");
				fflush(stdout);
			}
		}
		r = 0;
	} else {
		if (vflg > 0 && jflg == 0) {
			printf("Found ~%zu coprime pairs!!!\n", (p.used - (s1.used + s2.used)));
		}
		if (jflg > 0) {
			printf("{\"type\":\"interim result\",\"msg\":\"Found coprime pairs\",\"count\":%zu}\n", (p.used - (s1.used + s2.used)));
			fflush(stdout);
		}
		if (sflg == 0) {
			if (vflg > 0) {
				if (jflg == 0) {
					printf("Searching factors...\n");
				} else {
					printf("{\"type\":\"info\",\"msg\":\"Searching factors\"}\n");
					fflush(stdout);
				}
			}
			array_init(&out, 9);
			// Use [Algorithm 21.2](copri.html#factoring-a-set-over-a-coprime-base) to find the coprimes in the coprime base.
			array_find_factors(&pool, &out, &s1, &p);
			array_find_factors(&pool, &out, &s2, &p);
			// Output the factors.
			if (out.used > 0) {
				if ((out.used % 3) != 0) {
					fprintf(stderr, "Find factors returned an invalid array\n");
				} else {
					if (jflg > 0) {
						for(i = 0; i < out.used; i+=3)
							gmp_printf("{\"type\":\"result\",\"msg\":\"Found factors\",\"key\":\"%Zu\",\"p\":\"%Zu\",\"q\":\"%Zu\"}\n", out.array[i], out.array[i+1], out.array[i+2]);
					} else if (rflg > 0) {
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
	array_clear(&s1);
	array_clear(&s2);
	if (vflg > 0 && jflg == 0)
		pool_inspect(&pool);
	pool_clear(&pool);
	if (jflg > 0) {
		printf("{\"type\":\"end\",\"msg\":\"Finished\"}\n");
		fflush(stdout);
	}
	return r;
}
