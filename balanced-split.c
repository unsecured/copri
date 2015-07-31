// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// This file contains a balanced split util application
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
	mpz_array s, uniques, o;
	size_t count, chunk_count = 2, length, j, wc;
	int c, vflg = 0, lflg = 0, nflg = 0, errflg = 0, r = 0;
	char *filename = "primes.lst";
	char *out_filename = NULL;
	char chunk_name[MAX_CHUNK_NAME_LENGTH];
	long int level = 0, i, chunk_size_ui, index;
	unsigned int padding = 9;

	mpf_set_default_prec(64);

	// #### argument parsing
	// Boring `getopt` argument parsing.
	while ((c = getopt(argc, argv, ":vl:o:")) != -1) {
		switch(c) {
		case 'o':
			out_filename = optarg;
			break;
		case 'n':
			nflg++;;
			break;
		case 'v':
			vflg++;
			break;
		case 'l':
			lflg++;
			level = strtol(optarg, NULL, 0);
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

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg || lflg <= 0) {
		fprintf(stderr, "usage: [-v] [-o FILE] [-l LENGTH] [file]\n"\
                        "\n\t-o FILE   the output file prefix"\
						"\n\t-l LEVEL  the level of the tree"\
						"\n\t-n        do not sort and unique input"\
                        "\n\t-v        be more verbose"\
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

	if (nflg == 0) {
		if (vflg > 0)
			printf("sorting the input integers...\n");
		array_msort(&s);

		if (vflg > 0)
			printf("finding unique integers...\n");
		array_init(&uniques, 10);
		array_unique(&uniques, &s);
		printf("unique: %zu / %zu\n", uniques.used, s.used);

		count = uniques.used;
	} else {
		count = s.used;
	}

	for (i=1; i<level; i++) {
		chunk_count *= 2;
	}
	
	if (vflg > 0)
		printf("using %zu chunks\n", chunk_count);

	if (chunk_count > count) {
		fprintf(stderr, "More chunks then input integers\n");
		return 4;
	}

	mpz_t chunk_size, rest, size;

	mpz_init(chunk_size);
	mpz_init(rest);
	mpz_init_set_ui(size, count);

	mpz_fdiv_qr_ui(chunk_size, rest, size, chunk_count);

	gmp_printf("chunk_size: %Zu, rest: %Zu, size: %Zu, chunk_count: %zu\n", chunk_size, rest, size, chunk_count);

	chunk_size_ui = mpz_get_ui(chunk_size);

	mpf_t ratio, radio_at;

	mpf_init(ratio);

	mpf_set_z(ratio, rest);

	mpf_div_ui(ratio, ratio, chunk_count);

	gmp_printf("ratio: %Ff\n", ratio);

	mpf_init_set_ui(radio_at, 0);

	if (out_filename != NULL) {
		length = chunk_size_ui;

		// generate the chunks
		if (vflg > 0)
			printf("creating chunks with prefix '%s' and size: %zu\n", out_filename, length);
		array_init(&o, length);

		// get the padding size
		padding = snprintf ( chunk_name, MAX_CHUNK_NAME_LENGTH, "%zu", count);

		index = 0;
		for(i=0; i<chunk_count; i++) {

			length = chunk_size_ui;

			mpf_add(radio_at, radio_at, ratio);

			if (mpf_cmp_ui(radio_at, 1) >= 0) {
				length++;
				mpf_sub_ui(radio_at, radio_at, 1);
			}

			array_init(&o, length);

			for (j=0; j<length; j++) {
				if (nflg == 0) {
					array_add(&o, uniques.array[index+j]);
				} else {
					array_add(&o, s.array[index+j]);
				}
				
			}

			if (snprintf ( chunk_name, MAX_CHUNK_NAME_LENGTH, "%s_%0*zu-%0*zu.lst", out_filename, padding, index, padding, index+j) < 0) {
				fprintf(stderr, "Chunk name encoding error!\n");
				return 5;
			}
			printf("writing chunk '%s' size: %zu\n", chunk_name, length);


			wc = array_to_file(&o, chunk_name);
			if (o.used != wc) {
				fprintf(stderr, "Array size and write count do not match\n");
				return 4;
			}

			index += length;
			array_clear(&o);
		}
	}

	/*
var count = 26177420;

var desiredSize = 1000;

var desiredChunkCount = count/desiredSize;

var desiredLevel = Math.log(desiredChunkCount)/Math.log(2);

var realLevel = Math.round(desiredLevel);

console.log('level', desiredLevel, realLevel);

var realChunkCount = Math.pow(2, realLevel);

console.log('chunk count', desiredChunkCount, realChunkCount);

var realSize = Math.floor(count/realChunkCount);

console.log('size', desiredSize, realSize);

var rest = count - (realSize*realChunkCount);

var restRatio = rest/realChunkCount;

console.log('rest', rest, restRatio);
	 */

/*
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

	*/

	array_clear(&s);
	array_clear(&uniques);

	return r;
}
