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

#define MAX_CHUNK_NAME_LENGTH 256

// The generic `main` function.
//
// Define all variables at the beginning to make the C99 compiler
// happy.
int main(int argc, char **argv) {
	mpz_array s, m, b, uniques, filtered;
	mpz_t sum_bits, avg;
	size_t count, i, j, x, size, size_min = 0, size_max = 0;
	int c, vflg = 0, iflg = 0, sflg = 0, lflg = 0, bflg = 0, cflg = 0, uflg = 0, tflg = 0, xflg = 0, errflg = 0, r = 0;
	char *filename = "primes.lst";
	char *out_filename = NULL;
	char chunk_name[MAX_CHUNK_NAME_LENGTH];
	long int length = 0;
	long int seek = 0;
	long int tolerance = 0;
	long int bitsize = 0;
	unsigned int padding = 9;

	// #### argument parsing
	// Boring `getopt` argument parsing.
	while ((c = getopt(argc, argv, ":vsicux:t:b:l:o:")) != -1) {
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
		case 'i':
			iflg++;
			break;
		case 'u':
			uflg++;
			break;
		case 'c':
			cflg++;
			break;
		case 'x':
			xflg++;
			bitsize = strtol(optarg, NULL, 0);
			break;
		case 't':
			tflg++;
			tolerance = strtol(optarg, NULL, 0);
			break;
		case 'l':
			lflg++;
			length = strtol(optarg, NULL, 0);
			break;
		case 'b':
			bflg++;
			seek = strtol(optarg, NULL, 0);
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

	if (optind == argc - 1) {
		filename = argv[optind];
	} else {
		errflg++;
	}

	if (sflg > 0 && out_filename == NULL) {
		fprintf(stderr, "\n\t-s requires -o FILE!\n\n");
		errflg++;
	}

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg) {
		fprintf(stderr, "usage: [-vs] [-o FILE] [file]\n"\
						"\n\t-i        inspect the array"\
						"\n\t-o FILE   the output file"\
						"\n\t-l length max values to output or chunk size"\
						"\n\t-b count  skip first count (seek)"\
						"\n\t-c        create chunks"\
						"\n\t-v        be more verbose"\
						"\n\t-s        sort the input"\
						"\n\t-u        count uniques"\
						"\n\t-x bits   only output integers with size bits"\
						"\n\t-t bits   tolerance in bits for -x"\
						"\n\n");
		exit(2);
	}

	// Default to inspect.
	if (!sflg) {
		iflg++;
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

	if (out_filename != NULL && vflg > 0 && !(cflg && lflg)) {
		printf("output is going to be saved in '%s'\n", out_filename);
	}

	if (sflg > 0 || uflg > 0) {
		if (vflg > 0)
			printf("sorting the input integers...\n");
		array_msort(&s);
	}

	if (uflg > 0) {
		array_init(&uniques, 10);
		array_unique(&uniques, &s);
		printf("unique: %zu / %zu\n", uniques.used, s.used);
		array_clear(&uniques);
	}

	if (xflg > 0) {
		if (vflg > 0)
			printf("filter %zu integers by size %zu bits +/- %zu bits\n", s.used, bitsize, tolerance);

		array_init(&filtered, 10);
		for(i=0; i<s.used; i++) {
			size = mpz_sizeinbase(s.array[i], 2);
			if (size <= (bitsize + tolerance) && size >= (bitsize - tolerance)) {
				array_add(&filtered, s.array[i]);
			}
		}
		s = filtered;

	}

	if (iflg > 0) {
		printf("count: %zu\n", s.used);
		mpz_init_set_ui(sum_bits, 0);
		for(i=0; i<s.used; i++) {
			size = mpz_sizeinbase(s.array[i], 2);
			mpz_add_ui(sum_bits, sum_bits, size);
			if (size_min == 0 || size_min > size)
				size_min = size;
			if (size_max == 0 || size_max < size)
				size_max = size;

		}
		mpz_init(avg);
		mpz_cdiv_q_ui(avg, sum_bits, s.used);

		if (size_min == size_max) {
			printf("size (all): %zu bit\n", size_min);
		} else {
			gmp_printf("size:\n  - min: %zu bit\n  - max: %zu bit\n  - avg: %Zu bit\n", size_min, size_max, avg);
		}
		mpz_clear(avg);
		mpz_clear(sum_bits);

	}

	if (out_filename != NULL) {
		if (cflg && lflg) {
			// generate the chunks
			if (vflg > 0)
				printf("creating chunks with prefix '%s' and size: %zu\n", out_filename, length);
			b = s;
			s = m;
			array_init(&s, 10);
			j = 0;
			x = seek;

			// get the padding size
			padding = snprintf ( chunk_name, MAX_CHUNK_NAME_LENGTH, "%zu", b.used-seek);

			for(i=seek; i<=b.used; i++) {
				if (j >= length || i == b.used) {
					if (snprintf ( chunk_name, MAX_CHUNK_NAME_LENGTH, "%s_%0*zu-%0*zu.lst", out_filename, padding, x, padding, (i-1)) < 0) {
						fprintf(stderr, "Chunk name encoding error!\n");
						return 5;
					}
					printf("writing chunk '%s'\n", chunk_name);

					count = array_to_file(&s, chunk_name);
					if (s.used != count) {
						fprintf(stderr, "Array size and write count do not match\n");
						return 4;
					}

					array_clear(&s);
					array_init(&s, 10);

					j = 0;
					x = i;
				}
				if (i<b.used) {
					array_add(&s, b.array[i]);
					j++;
				}
			}
			array_clear(&b);
		} else {
			// just process seek and length
			if (vflg > 0)
				printf("storing output in '%s'\n", out_filename);

			if (lflg || bflg) {
				b = s;
				s = m;
				array_init(&s, 10);
				j = 0;
				for(i=seek; i<b.used; i++) {
					if (lflg) {
						if (j >= length) break;
						j++;
					}
					array_add(&s, b.array[i]);
				}
				array_clear(&b);
			}

			count = array_to_file(&s, out_filename);
			if (s.used != count) {
				fprintf(stderr, "Array size and write count do not match\n");
				return 4;
			}
		}
	}

	array_clear(&s);

	return r;
}
