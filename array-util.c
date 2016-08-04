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
	mpz_array s, uniques, filtered, seekedLength, sample;
	mpz_t sum_bits, avg;
	size_t count, i, j, size, size_min = 0, size_max = 0;
	int c, vflg = 0, iflg = 0, sflg = 0, lflg = 0, bflg = 0, rflg = 0, uflg = 0, tflg = 0, xflg = 0, jflg = 0, errflg = 0, r = 0;
	char *filename = "primes.lst";
	char *out_filename = NULL;
	long int length = 0;
	long int seek = 0;
	long int tolerance = 0;
	long int bitsize = 0;
	// for random sampling
	long int sample_size = 0;
	gmp_randstate_t randstate;
	mpz_t r_index;
	mpz_t r_max;
	size_t *used_indices;
	int found = 0;

	// #### argument parsing
	// Boring `getopt` argument parsing.
	while ((c = getopt(argc, argv, ":vsiujr:x:t:b:l:o:")) != -1) {
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
		case 'j':
			jflg++;
			break;
		case 'r':
			rflg++;
			sample_size = strtol(optarg, NULL, 0);
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
						"\n\t-r length create random sample"\
						"\n\t-v        be more verbose"\
						"\n\t-s        sort the input"\
						"\n\t-u        count uniques"\
						"\n\t-j        print json array"\
						"\n\t-x bits   only output integers with size bits"\
						"\n\t-t bits   tolerance in bits for -x"\
						"\n\n");
		exit(2);
	}

	// Default to inspect.
	if (!sflg && !jflg && !rflg) {
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

	if (out_filename != NULL && vflg > 0 && !jflg) {
		printf("output is going to be saved in '%s'\n", out_filename);
	}

	// # preprocessing functions

	if (sflg > 0 || uflg > 0) {
		if (vflg > 0)
			printf("sorting the input integers...\n");
		array_msort(&s);
	}

	// # filter functions

	// filter per bit size
	if (xflg > 0) {
		if (vflg > 0) {
			printf("filter %zu integers by size %zu bits +/- %zu bits\n", s.used, bitsize, tolerance);
		}
		array_init(&filtered, 10);
		for(i=0; i<s.used; i++) {
			size = mpz_sizeinbase(s.array[i], 2);
			if (size <= (bitsize + tolerance) && size >= (bitsize - tolerance)) {
				array_add(&filtered, s.array[i]);
			}
		}
		array_clear(&s);
		s = filtered;
	}

	// create random sample
	if (rflg > 0) {
		if (s.used < sample_size) {
			fprintf(stderr, "Sample size %zu is bigger then input size %zu\n", sample_size, s.used);
			return 6;
		}
		if (sample_size <= 0) {
			fprintf(stderr, "Sample size is %zu\n", sample_size);
			return 6;
		}
		if (vflg > 0) {
			printf("creating random sample of %zu from %zu integers\n", sample_size, s.used);
		}
		gmp_randinit_default(randstate);
		mpz_init_set_ui(r_max, s.used);
		mpz_init(r_index);
		array_init(&sample, 10);
		used_indices = (size_t *)malloc(sample_size * sizeof(size_t));
		while(sample.used < sample_size) {
			mpz_urandomm(r_index, randstate, r_max);
			i = mpz_get_ui(r_index);
			found = 0;
			for (j=0; j<sample.used; j++) {
				if (used_indices[j] == i) {
					found = 1;
					break;
				}
			}
			if (!found) {
				used_indices[sample.used] = i;
				array_add(&sample, s.array[i]);
			}
		}
		free(used_indices);
		gmp_randclear(randstate);
		mpz_clear(r_index);
		mpz_clear(r_max);
		array_clear(&s);
		s = sample;
	}

	// filter per seek and length
	if (lflg || bflg) {
		if (vflg > 0) {
			printf("seeking to %zu and limiting to %zu integers\n", seek, length);
		}
		array_init(&seekedLength, 10);
		j = 0;
		for(i=seek; i<s.used; i++) {
			if (lflg) {
				if (j >= length) break;
				j++;
			}
			array_add(&seekedLength, s.array[i]);
		}
		array_clear(&s);
		s = seekedLength;
	}

	// # Output functions

	// print info
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

	// print JSON
	if (jflg > 0) {
		printf("[\n");
		for(i=0; i<s.used; i++) {
			if (i == (s.used - 1)) {
				gmp_printf("\"%Zu\"\n", s.array[i]);
			} else {
				gmp_printf("\"%Zu\",\n", s.array[i]);
			}
		}
		printf("]\n");
	}

	// print uniques
	if (uflg > 0) {
		array_init(&uniques, 10);
		array_unique(&uniques, &s);
		printf("unique: %zu / %zu\n", uniques.used, s.used);
		array_clear(&uniques);
	}

	// save as file
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
