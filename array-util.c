// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This file contains a array util application
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <gmp.h>
#include "copri.h"

// The generic `main` function.
//
// Define all variables at the beginning to make the C99 compiler
// happy.
int main(int argc, char **argv) {
	mpz_array s;
	size_t count;
	int c, vflg = 0, sflg = 0, errflg = 0, r = 0;
	char *filename = "primes.lst";
	char *out_filename = NULL;

	// #### argument parsing
	// Boring `getopt` argument parsing.
	while ((c = getopt(argc, argv, ":vso:")) != -1) {
		switch(c) {
		case 'o':
			out_filename = optarg;
			break;
		case 's':
			sflg++;
			break;
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

	if (sflg > 0 && out_filename == NULL) {
		fprintf(stderr, "\n\t-s requires -o FILE!\n\n");
		errflg++;
	}

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg) {
		fprintf(stderr, "usage: [-vs] [-o FILE] [file]\n"\
                        "\n\t-o FILE   the output file"\
                        "\n\t-v        be more verbose"\
                        "\n\t-s        sort the input"\
                        "\n\n");
		exit(2);
	}

	// Load the integers.
	array_init(&s, 10);
	count = array_of_file(&s, filename);
	if (count == 0) {
		fprintf(stderr, "Can't load %s\n", filename);
		return 1;
	}
	if (s.used != count) {
		fprintf(stderr, "Array size and load count do not match\n");
		return 2;
	}
	if (s.used == 0) {
		fprintf(stderr, "No integers loaded (empty file)\n");
		return 3;
	}

	if (out_filename != NULL && vflg > 0) {
		printf("output is going to be saved in '%s'\n", out_filename);
	}

	if (sflg > 0) {
		if (vflg > 0)
			printf("sorting the input integers...\n");
		array_msort(&s);
	}

	if (out_filename != NULL) {
		if (vflg > 0)
			printf("storing output in '%s'\n", out_filename);
		count = array_to_file(&s, out_filename);
		if (s.used != count) {
			fprintf(stderr, "Array size and write count do not match\n");
			return 4;
		}
	}

	array_clear(&s);

	return r;
}
