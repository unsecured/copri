// copri, Attacking RSA by factoring coprimes
//
// License: GNU Lesser General Public License (LGPL), version 3 or later
// See the lgpl.txt file in the root directory or <https://www.gnu.org/licenses/lgpl>.

// Read public_key.csv from [scans.io](https://scans.io/study/umich-https) and convert it to
// copri compatible raw gmp format.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gmp.h>

#define READ_BUF_SIZE 8192
#define DEFAULT_MIN_LENGTH 512
#define DEFAULT_MAX_LENGTH 8192

typedef void (*readline_cb_t)(char* line, unsigned int len);
void readline(FILE *fp, readline_cb_t readline_cb, unsigned int read_buffer_size);
void readline_cb(char* line, unsigned int len);

FILE *out;
long int min_length = DEFAULT_MIN_LENGTH;
long int max_length = DEFAULT_MAX_LENGTH;
size_t key_count = 0;

// Define the variables set by the argument parser
int c, vflg = 0, errflg = 0;

// # main

// Read the command line arguments, open the output file and the csv file then
// start appending rsa key `n` values read from csv in raw gmp format to the
// output file.
int main (int argc, char *argv[]) {
	FILE *csv;

	char* csv_filename = "-";
	char* out_filename = "-";

	// Read the command line arguments.
	while ((c = getopt(argc, argv, ":vcpg:l:o:")) != -1) {
		switch(c) {
		case 'v':
			vflg++;
			break;
		case 'g':
			min_length = strtol(optarg, NULL, 0);
			if (min_length < 1) errflg++;
			break;
		case 'l':
			max_length = strtol(optarg, NULL, 0);
			if (max_length < 1) errflg++;
			break;
		case 'o':
			out_filename = optarg;
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
		csv_filename = argv[optind];
		if (optind + 1 < argc) errflg++;
	}

	// Print the usage and exit if an error occurred during argument parsing.
	if (errflg) {
		fprintf(stderr, "usage: [-vc] [-g min_keylength] [-l max_keylength] [-o gmp_file] [csv_file]\n"\
						"\n\t-v        be more verbose"\
						"\n"\
						"\n\t-g MIN_KEYLENGTH    the minimal key length (>=)"\
						"\n\t-l MAX_KEYLENGTH    the maximum key length (<=)"\
						"\n\t-o GMP_FILE         the gmp file to write"\
						"\n\n");
		exit(2);
	}

	// Set `out` to `stdout` if the filename is `-`.
	if (strcmp(out_filename, "-") == 0) {
		out = stdout;
		// Print the used parameters if the verbose flag is set.
		if (vflg > 0) {
			fprintf(stderr, "Printing RSA n values in raw gmp format to stdout.\n");
		}
	} else {
		// Open the output file.
		out = fopen(out_filename, "a+");
		if (out == NULL) {
			printf("Cannot save to file %s.\n", out_filename);
			exit(3);
		}
		// Print the used parameters if the verbose flag is set.
		if (vflg > 0) {
			fprintf(stderr, "Adding RSA n values to file %s.\n", out_filename);
		}
	}

	// Set `csv` to `stdin` if the filename is `-`.
	if (strcmp(csv_filename, "-") == 0) {
		csv = stdin;
		// Print the used parameters if the verbose flag is set.
		if (vflg > 0) {
			fprintf(stderr, "Reading CSV from stdin.\n");
		}
	} else {
		// Open the csv file.
		csv = fopen(csv_filename, "rt");
		if (csv == NULL) {
			printf("Cannot read %s.\n", csv_filename);
			exit(4);
		}
		// Print the used parameters if the verbose flag is set.
		if (vflg > 0) {
			fprintf(stderr, "Reading CSV from %s.\n", csv_filename);
		}
	}

	// This calls `readline_cb` on every line of `csv`.
	readline(csv, readline_cb, READ_BUF_SIZE);

	// Colse the files.
	fclose(csv);
	fclose(out);

	// Print the summary.
	if (vflg > 0) {
		fprintf(stderr, "%zu raw gmp keys have been writen to %s.\n", key_count, out_filename);
	}

	return 0;
}

void readline_cb(char* line, unsigned int len) {
	size_t write_len = 0;
	unsigned int c = 0;
	unsigned int c_start = 0;
	unsigned int c_end = 0;
	int is_rsa = 0;
	long int keylength = 0;
	mpz_t key;

	// Parse the line.
	for (c_end = 0; c_end<=len; c_end++) {
		if (line[c_end] == ',' || c_end == len) {
			line[c_end] = '\0';
			// Check if the second column contains "rsaEncryption".
			if (c == 1) {
				if (strncmp(line+c_start, "rsaEncryption", c_end-c_start) == 0) {
					is_rsa = 1;
				}
			// Store the fourth column as `mpz_t`.
			} else if(is_rsa && c == 4) {
				mpz_init(key);
				mpz_set_str(key, line+c_start+2, 16);
			// Store the tenth column as the key length.
			} else if(is_rsa && c == 9) {
				keylength = strtol(line+c_start, NULL, 0);
			}
			c++;
			c_start = c_end+1;
		}
	}

	// If the is an RSA key and the key length condition is meet write the
	// key in raw gmp format to `out`.
	if (is_rsa && keylength >= min_length && keylength <= max_length) {
		write_len = mpz_out_raw(out, key);
		if (write_len == 0) {
			fprintf(stderr, "Cannot write to file.\n");
		} else {
			key_count++;
		}
	}

	// Free the memory.
	if (is_rsa) {
		mpz_clear(key);
	}
}

// # readline
//
// read the file and call `readline_cb` on every line.
void readline(FILE *fp, readline_cb_t readline_cb, unsigned int read_buffer_size) {
	char *line_buf;
	unsigned int line_buf_size = 1024;
	unsigned int line_buf_index = 0;
	char read_buf[read_buffer_size];
	unsigned int line_start = 0;
	unsigned int line_end = 0;
	unsigned int line_chunck_size = 0;
	unsigned int i=0;
	size_t bytes_read = 0;

	// Alloc the line buffer by guessed size.
	line_buf = malloc(line_buf_size);
	for (unsigned int x=0; x<line_buf_size; x++)
		line_buf[x] = '?';

	// Start looping until the end of the file is reached.
	while(!feof(fp))
	{
		// Read the next `read_buffer_size` chunck of the file.
		bytes_read = fread(&read_buf, 1, read_buffer_size, fp);

		// Loop until the end of the chunck.
    	while (line_end < bytes_read)
    	{
    		// Find the index of the next `\n` char.
    		while (line_end < bytes_read && read_buf[line_end] != '\n')
    			line_end++;

    		// Store the size of line chunck.
    		line_chunck_size = line_end - line_start;

    		// Increase the size of the line buffer if needed.
			while (line_buf_index+line_chunck_size+1 > line_buf_size)
			{
				line_buf_size *= 2;
				line_buf = realloc(line_buf, line_buf_size);
			}

			// Copy the read buffer to the line buffer.
			for (i=0; i<line_chunck_size; i++)
				line_buf[line_buf_index+i] = read_buf[line_start+i];

			// Set the new line start index and terminate the string by `\0`.
			line_buf_index += i;
			line_buf[line_buf_index] = '\0';

			// Call the readline function if the line buffer ends with `\n`
			if (read_buf[line_end] == '\n') {
				readline_cb(line_buf, line_buf_index);
				line_buf_index = 0;
			}

			// Move the chunck to the next index.
    		line_end++;
    		line_start = line_end;
    	}

    	// Reset the line indices.
    	line_start = 0;
    	line_end = 0;

    }

	// Free the line buffer.
	free(line_buf);
}
