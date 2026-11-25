/*
 * A simple object file allocator
 *
 * The current version only addresses .text, .data, and .bss segments.
 * 	.text starts at 0x1000
 * 	.data starts at 0x1000 + sizeof(.text) -> aligned to 0x1000
 * 	.bss starts at sizeof(.data) -> 0x04 byte aligned
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
#include <allocator.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("%s: %s <input1.txt> <input2.txt> .. <ouput.o>\n",
		       argv[0], argv[0]);
		return 1;
	}
	FILE *input_files[argc - 2];
	FILE *output;

	// Open input files
	for (int i = 0; i < argc - 2; i++) {
		input_files[i] = fopen(argv[i + 1], "r");
		if (!input_files[i]) {
			printf("Error opening %s\n", argv[i + 1]);
			return 1;
		}
	}

	// Open the output file
	output = fopen(argv[argc - 1], "w");

	if (!output) {
		printf("Error opening %s\n", argv[argc - 1]);
		return 1;
	}

	initialize_buffers();

	int *meta[argc - 2];
	// Pass 1
	for (int i = 0; i < argc - 2; i++) {
		process_magic(input_files[i]);
		// Contains our meta data (no. segs, syms, relos)
		meta[i] = (int *)calloc(argc - 2, sizeof(int));
		meta[i] = process_metadata(input_files[i], output);
		if (*meta == NULL)
			return 1;
		process_segments(meta[i], input_files[i], output);

		process_symbols(meta[i], input_files[i], output);

		process_rels(meta[i], input_files[i], output);

		process_data(meta[i], input_files[i], output);

		set_file_allocation_data(meta[i], argv[i + 1]);
	}

	// Pass 2
	// Allocate and add segments in the output file
	allocate(output);

	// Cleanup
	for (int i = 0; i < argc - 2; i++) {
		free(meta[i]);
		fclose(input_files[i]);
	}
	fclose(output);
	free_buffers();
	clean_segments_list();

	return EXIT_SUCCESS;
}
