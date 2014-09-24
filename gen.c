// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// Generate openssl rsa keys and append it as gmp mpz binary integers to a file.
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>

// ## defaults
// Define the default values for omitted command line arguments.
#define DEFAULT_COUNT	    1000
#define DEFAULT_KEY_LENGTH  2048  // >=1024
#define DEFAULT_PUB_EXP     65537 // 3, 5, 17, 257 other Fermat primes.

// ## helpers

// Transform an `SHA1` hash to an hex string.
void SHA1BinaryToHex(const unsigned char *bin, char *hex) {
	int i;
	for(i = 0; i<20; ++i) {
		sprintf(&hex[i*2], "%02X", bin[i]);
	}
	hex[40]=0;
}

// # main

// Read the command line arguments, open the output file and start appending rsa key `n` values to the file.
int main (int argc, char *argv[]) {

	// Define the variables set by the argument parser
	int c, vflg = 0, sflg = 0, oflg = 0, fflg = 0, errflg = 0;
	long int count = DEFAULT_COUNT;
	long int keylength = DEFAULT_KEY_LENGTH;
	long int e = DEFAULT_PUB_EXP;
	char *filename = "primes.lst";

	// and the variables used by the key generation loop.
	FILE *out;
	RSA *keypair;
	mpz_t n, p, q;
	int i = 0;
	char *nstr;
	size_t buff_len = 0;
	unsigned char hash[SHA_DIGEST_LENGTH];
	char hex[SHA_DIGEST_LENGTH*2+1];

	// Read the command line arguments.
	while ((c = getopt(argc, argv, ":svofc:k:e:")) != -1) {
		switch(c) {
		case 's':
			sflg++;
			break;
		case 'o':
			oflg++;
			break;
		case 'v':
			vflg++;
			break;
		case 'f':
			fflg++;
			break;
		case 'c':
			count = strtol(optarg, NULL, 0);
			if (count < 1) errflg++;
			break;
		case 'k':
			keylength = strtol(optarg, NULL, 0);
			if (keylength < 512) errflg++;
			break;
		case 'e':
			e = strtol(optarg, NULL, 0);
			if (e < 3) errflg++;
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
		fprintf(stderr, "usage: [-vsof] [-c count] [-k keylength] [-e exponent] [file]\n"\
						"\n\t-v        be more verbose"\
						"\n\t-s        print sha1 hash representation"\
						"\n\t-o        print base 10 representation"\
						"\n\t-f        include one factorizable key"\
						"\n"\
						"\n\t-c COUNT        the amount of keys"\
						"\n\t-k KEYLENGTH    the key length"\
						"\n\t-e EXPONENT     the exponent of the keys"\
						"\n\n");
		exit(2);
	}

	// Set `out` to `stdout` if the filename is `-`.
	if (strcmp(filename, "-") == 0) {
		out = stdout;
		// Print the used parameters if the verbose flag is set.
		if (vflg > 0) {
			fprintf(stderr, "Printing %ld RSA n values to stdout.\nThe key length is %ld bit, e: %ld.\n", count, keylength, e);
		}
	} else {
		// Open the output file.
		out = fopen(filename, "a+");
		if (out == NULL) {
			printf("Cannot save to file %s.\n", filename);
			exit(3);
		}
		// Print the used parameters if the verbose flag is set.
		if (vflg > 0) {
			fprintf(stderr, "Adding %ld RSA n values to file %s.\nThe key length is %ld bit, e: %ld.\n", count, filename, keylength, e);
		}
	}

	// Initialize the the integer `n` and start the generation loop.
	mpz_init(n);
	for (i = 0; i < count; i++) {
		// Generate the `keypair` with openssl.
		keypair = RSA_generate_key(keylength, e, NULL, NULL);

		if (fflg && i == 0) {
			nstr = BN_bn2dec(keypair->p);
			mpz_init(p);
			mpz_set_str(p, nstr, 10);
			free(nstr);
		}

		if (fflg && (i+1) == count) {
			nstr = BN_bn2dec(keypair->q);
			mpz_init(q);
			mpz_set_str(q, nstr, 10);
			free(nstr);
			mpz_mul(n, p, q);
			nstr = mpz_get_str(NULL, 10, n);
			mpz_clear(p);
			mpz_clear(q);
		} else {
			// Convert it to a base 10 string.
			nstr = BN_bn2dec(keypair->n);
		}

		// Check if we should print the sha1 hash.
		if (sflg > 0) {
			// Build the SHA1 hash of the base 10 string.
			SHA1((unsigned char *)nstr, sizeof(nstr), hash);
			// Convert the hash to base16.
			SHA1BinaryToHex(hash, hex);
			// Print the sha1 hash.
			fprintf(stderr, "writing sha1: %s\n", hex);
		}
		// Print the base 10 string to stdout if the `o` option is set.
		if (oflg > 0) {
			fprintf(stderr, "%s\n", nstr);
		}
		// Convert the base 10 string to an mpz.
		mpz_set_str(n, nstr, 10);
		// Append the raw mpz with 4 bytes of size information.
		buff_len = mpz_out_raw(out, n);
		if (buff_len == 0) {
			printf("Cannot write to file %s.\n", filename);
		}

		// Free memory.
		free(nstr);
		RSA_free(keypair);
	}
	mpz_clear(n);
	// Close the output file.
	fclose(out);
	return 0;
}
