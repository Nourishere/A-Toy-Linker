/*
 * Simple UNIX-style storage allocation
 *
 * Storage allocation is the step in which the linker decides the memory
 * layout of the final output file.
 *
 * NOTE: The current version only addresses .text, .data, and .bss segments.
 * 	.text starts at 0x1000
 * 	.data starts at 0x1000 + sizeof(.text) -> aligned to 0x1000
 * 	.bss starts at sizeof(.data) -> 0x04 byte aligned
 */

#include <allocator.h>
#include <parser.h>
#include <ds.h>
#include <stdlib.h>
#include <string.h>

#define NAME_BUFF_LEN 128

#define ALIGN_UP(value, alignment) \
		(((value) + (alignment) - 1) & ~((alignment) - 1))

segments_list *seg_list = 0;

// The global segments double array
extern char **segments;

// The global data array (data per file)
extern uint32_t *data;

static int add_segment(void);

void set_file_allocation_data(int *meta, char *file_name)
{
	int line_offset;
	if (!meta) {
		puts("Error procssing meta data");
		exit(1);
	}
	// We have the meta array and implicitly the segments double array set
	if (!segments)
		exit(1);

	add_segment();

	segments_list *tail = seg_list;
	while (tail->next)
		tail = tail->next;

	// Copy file name
	strncpy(tail->file_name, file_name, strlen(file_name));

	char **temp = segments;
	for (int i = 0; temp[i] && i < meta[NOSEGS]; i++) {
		// Copy segment name
		strncpy(tail->name[i], &segments[i][NOSEGS],
			strlen(&segments[i][NOSEGS]));
		// Copy address
		line_offset = index_buffer(NOSYMS, segments[i]);
		tail->load_addr[i] = atoi(&segments[i][line_offset]);
		// Copy size
		line_offset = index_buffer(2, segments[i]);
		tail->size[i] = atoi(&segments[i][line_offset]);
		// Copy data
		tail->data[i] = data[i];
	}

}

// Calculate and write segment data to output file
void allocate(FILE *obj)
{
	uint32_t text_start;
	uint32_t data_start;
	uint32_t bss_start;
	uint32_t text_size = 0;
	uint32_t data_size = 0;

	segments_list *temp = seg_list;
	while (temp) {
		text_size += temp->size[0];
		data_size += temp->size[1];

		temp = temp->next;
	}
	text_start = 0x1000;
	data_start = ALIGN_UP(text_start + text_size, 0x1000);
	bss_start = ALIGN_UP(data_start + data_size, 0x04);

	// Write .text
	fseek(obj, text_start, SEEK_SET);
	temp = seg_list;
	while (temp) {
		uint32_t text = temp->data[0];
		while (text) {
			fputc(text & 0xFF, obj);
			text >>= 8;
		}
		temp = temp->next;
	}

	// Write .data
	fseek(obj, data_start, SEEK_SET);
	temp = seg_list;
	while (temp) {
		uint32_t data = temp->data[1];
		while (data) {
			fputc(data & 0xFF, obj);
			data >>= 8;
		}
		temp = temp->next;
	}

	// Write .bss
	fseek(obj, bss_start, SEEK_SET);
	temp = seg_list;
	while (temp) {
		uint32_t bss = temp->data[2];
		temp = temp->next;
		while (bss) {
			fputc(bss & 0xFF, obj);
			bss >>= 8;
		}
	}

}

// Clean the segments linked list
int clean_segments_list(void)
{
	segments_list *temp = seg_list;
	segments_list *temp_next;
	while (temp) {
		for (int i = 0; i < NUM_SEGS_PER_FILE; i++) {
			free(temp->name[i]);
		}
		free(temp->file_name);
		free(temp->load_addr);
		free(temp->name);
		free(temp->size);
		free(temp->data);

		temp_next = temp->next;
		free(temp);
		temp = temp_next;
	}
	return 0;
}

static int add_segment(void)
{
	segments_list *temp, *new;
	if (seg_list == 0) {	// first node
		seg_list = (segments_list *) malloc(sizeof(segments_list));
		if (!seg_list)
			return 1;
		seg_list->next = 0;
		new = seg_list;
	} else {
		temp = seg_list;
		while (temp->next)
			temp = temp->next;
		new = (segments_list *) malloc(sizeof(segments_list));
		if (!new)
			return 1;
		temp->next = new;
		new->next = 0;
	}
	new->data = (uint32_t *) calloc(MAX_NUM_SEGS, sizeof(uint32_t));
	new->file_name = (char *)calloc(NAME_BUFF_LEN, sizeof(char));

	// Allocate space for all segment names in the given input file
	new->name = (char **)calloc(NUM_SEGS_PER_FILE, sizeof(char *));
	for (int i = 0; i < NUM_SEGS_PER_FILE; i++)
		new->name[i] = (char *)calloc(NAME_BUFF_LEN, sizeof(char));

	// Allocate space for the array of sizes and array of addresses
	new->size = (uint32_t *) calloc(NUM_SEGS_PER_FILE, sizeof(uint32_t));
	new->load_addr =
	    (uint32_t *) calloc(NUM_SEGS_PER_FILE, sizeof(uint32_t));
	if (!new || !new->name || !new->file_name)
		return 1;

	return 0;
}
