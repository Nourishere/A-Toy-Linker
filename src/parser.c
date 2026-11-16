/*
 *
 *
 */

#include <parser.h>
#include <ds.h>
#include <string.h>
#include <stdlib.h>

#define NOSEGS 0
#define NOSYMS 1
#define NORELS 2

#define INPUT_BUFFER_SIZE 1024
#define OUTPUT_BUFFER_SIZE 1024
#define SEGMENT_BUFFER_SIZE 1024

static const char *no[3] = { "Number of segments: ", "Number of symbols: ",
	"Number of relocations: "
};

static char *input_buffer;
static char *output_buffer;

int initialize_buffers(void)
{
	input_buffer = calloc(INPUT_BUFFER_SIZE, sizeof(char));
	output_buffer = calloc(OUTPUT_BUFFER_SIZE, sizeof(char));
	segments = (char **)malloc(3 * sizeof(char *));
	if (!segments)
		return 1;
	for (int i = 0; i < 3; i++)
		segments[i] = calloc(SEGMENT_BUFFER_SIZE, sizeof(char));

	if (!segments[0] || !segments[1] || !segments[2] || !input_buffer
	    || !output_buffer)
		return 1;
	return 0;
}

void free_buffers(void)
{
	free(input_buffer);
	free(output_buffer);
	for (int i = 0; i < 3; i++)
		free(segments[i]);
	free(segments);
}

int process_magic(FILE *input)
{
	if (fread((char *)input_buffer, 1, 5, input) != 5) {
		printf("Cannot find magic word\n");
		return 1;
	}
	if (strncmp("LINK\n", input_buffer, 5)) {
		printf("Cannot find magic word\n");
		return 1;
	}
	return 0;
}

int *process_metadata(FILE *input, FILE *output)
{
	int j;
	int *meta = calloc(3, sizeof(int));

	char *cycle[3] = { "segments", "symbols", "relocations" };
	for (int i = 0; i < 3; i++) {
		for (j = 0;; j++) {
			fread(&input_buffer[j], 1, 1, input);
			if (input_buffer[j] == ' ' || input_buffer[j] == '\n')
				break;
		}
		input_buffer[j++] = '\0';
		meta[i] = atoi(input_buffer);
		if (meta[i] == 0) {
			printf("Error reading %s\n", cycle[i]);
			return 0;
		}

		fwrite(no[i], sizeof(char), strlen(no[i]), output);
		fwrite(input_buffer, sizeof(char), strlen(input_buffer),
		       output);
		fwrite("\n", 1, 1, output);
		flush_input_buffer();
	}
	fwrite("\n", 1, 1, output);
	return meta;
}

int process_segments(int *meta, FILE *input, FILE *output)
{
	int i, j;
	int token_index;
	char *cycle[4] = { "name: ", "adderss: ", "size: ", "flags: " };

	fwrite("---- Segments ----\n", 1, 19, output);
	for (i = 0; i < meta[NOSEGS]; i++) {
		process_line(input, input_buffer);

		for (j = 0; j < 4; j++) {
			fwrite(cycle[j], 1, strlen(cycle[j]), output);
			token_index = index_buffer(j, input_buffer);
			// Store segment information for later use
			int offset = index_buffer(j, segments[i]);
			strncpy(&segments[i][offset],
				&input_buffer[token_index],
				strlen(&input_buffer[token_index]));

			if (j > 0 && j < 3) {
				if (atoi(&input_buffer[token_index]) == 0) {
					printf("Expected integer for %s\n",
					       (j == 1 ? "address" : "size"));
					return 1;
				}
			}
			fwrite(&input_buffer[token_index], 1,
			       strlen(&input_buffer[token_index]), output);
			fwrite("\n", 1, 1, output);
		}

		fwrite("\n", 1, 1, output);
	}
	for (i = 0; i < 3; i++)
		tokenize(segments[i]);
	return 0;
}

int process_symbols(int *meta, FILE *input, FILE *output)
{
	int i, j;
	int seg_num;
	int token_index;
	char *cycle[4] = { "Name: ", "Value: ", "Segment: ", "Type: " };
	fwrite("---- Symbols ----\n", 1, 18, output);
	for (i = 0; i < meta[NOSYMS]; i++) {
		process_line(input, input_buffer);
		for (j = 0; j < 4; j++) {
			fwrite(cycle[j], 1, strlen(cycle[j]), output);
			token_index = index_buffer(j, input_buffer);
			if (j == 0 && atoi(&input_buffer[j]) != 0) {
				printf("Symbol name cannot be a number\n");
				return 1;
			}
			seg_num = atoi(&input_buffer[token_index]);
			if (j == 1 && seg_num == 0
			    && strncmp(&input_buffer[token_index], "0", 1)) {
				printf("Expected integer for value\n");
				return 1;
			} else if (j == 2) {
				if (seg_num > meta[NOSEGS]) {
					printf("Segment not found\n");
					return 1;
				}

				token_index =
				    index_buffer(seg_num, segments[i]);
				fwrite(&segments[seg_num][0], 1,
				       strlen(&segments[seg_num][0]), output);
				fwrite("\n", 1, 1, output);
				continue;

			}
			fwrite(&input_buffer[token_index], 1,
			       strlen(&input_buffer[token_index]), output);
			fwrite("\n", 1, 1, output);
		}
		fwrite("\n", 1, 1, output);
	}

	return 0;
}

int process_rels(int *meta, FILE *input, FILE *output)
{
	int i, j;
	int s_number;
	int token_index;
	char *cycle[4] = { "Location: ", "Segment: ", "Reference: ", "Type: " };
	fwrite("---- Relocations ----\n", 1, 22, output);
	for (i = 0; i < meta[NORELS]; i++) {
		process_line(input, input_buffer);
		for (j = 0; j < 4; j++) {
			fwrite(cycle[j], 1, strlen(cycle[j]), output);
			token_index = index_buffer(j, input_buffer);
			if (j == 0 && !atoi(&input_buffer[j])
			    && strncmp(&input_buffer[j], "0", 1)) {
				printf("Invalid relocation location\n");
				return 1;
			} else if (j == 1 || j == 2) {
				// Because atoi doesn't differentiate between
				// a "0" char and an error return
				s_number = atoi(&input_buffer[token_index]);
				if (s_number == 0
				    && strncmp(&input_buffer[token_index], "0",
					       1)) {
					printf("Expected integer for value\n");
					return 1;
					// Fast way to get the index from j
				} else if (s_number > meta[j - 1]) {
					printf("%s not found\n",
					       (j == 1 ? "Segment" : "Symbol"));
					return 1;
				}
			}
			if (j == 1) {
				int seg_num = atoi(&input_buffer[token_index]);
				fwrite(&segments[seg_num][0], 1,
				       strlen(&segments[seg_num][0]), output);
				fwrite("\n", 1, 1, output);
			} else {
				fwrite(&input_buffer[token_index], 1,
				       strlen(&input_buffer[token_index]),
				       output);
				fwrite("\n", 1, 1, output);
			}
		}
		fwrite("\n", 1, 1, output);
	}

	return 0;
}

int process_data(int *meta, FILE *input, FILE *output)
{
	int i, j = 0;
	fwrite("---- Data ----\n", 1, 15, output);
	for (i = 0; i < meta[NOSEGS]; i++) {
		process_line(input, input_buffer);
		fwrite(&segments[i][0], 1, strlen(&segments[i][0]), output);
		fwrite(": ", 1, 2, output);
		process_line(input, input_buffer);
		fwrite(&input_buffer[j], 1, strlen(&input_buffer[j]), output);

		fwrite("\n", 1, 1, output);
	}

	return 0;
}

void flush_input_buffer(void)
{
	for (int i = 0; i < INPUT_BUFFER_SIZE; i++)
		input_buffer[i] = '\0';
}

// Clean up allocated memory and wipe the output file
// This should be called upon failure
void cleanup(void)
{

	free(input_buffer);
	free(output_buffer);

	exit(1);
}

// Process a line from input file into the input buffer
// This function tokenizes by inserting '\0'
void process_line(FILE *input, char *buf)
{
	for (int i = 0;; i++) {
		fread(&buf[i], 1, 1, input);
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
	}
	tokenize(buf);
}

void tokenize(char *buf)
{
	int i = 0;
	while (buf[i] != '\0') {
		if (buf[i] == ' ')
			buf[i] = '\0';
		i++;
	}
}

// Get the ith token in the input buffer
// Token index starts from zero
int index_buffer(int token_number, char *buf)
{
	int i = 0;
	if (token_number == 0)
		return 0;
	while (token_number--) {
		while (buf[i++] != '\0') ;
	}
	return i;
}
