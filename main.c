/*
 * A simple object file parser
 *
 * Format:
 *
 * 	LINK
 *	nsegs nsyms nrels
 *	- segments -
 *	- symbols -
 *	- rels -
 *	- data -
 */

#include <parser.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("%s: %s <input.txt> <ouput.txt>\n", argv[0], argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");
	if (errno == EINVAL) {
		perror("Error opening input file\n");
	}
	FILE *output = fopen(argv[2], "w");
	if (errno == EINVAL) {
		perror("Error opening output file\n");
	}

	if (initialize_buffers()) {
		printf("Error initializing buffers\n");
		return 1;
	}

	if (process_magic(input))
		return 1;

	// Contains our meta data (no. segs, syms, relos)
	int *meta = process_metadata(input, output);

	if (meta == NULL)
		return 1;

	if (process_segments(meta, input, output))
		return 1;

	if (process_symbols(meta, input, output))
		return 1;

	if (process_rels(meta, input, output))
		return 1;

	if (process_data(meta, input, output))
		return 1;

	free_buffers();
	free(meta);
	fclose(input);
	fclose(output);

	return EXIT_SUCCESS;
}
