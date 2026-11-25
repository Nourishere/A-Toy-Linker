/*
 * Internal common data structure and macros
 */

#ifndef __DS_H
#define __DS_H

#include <stdint.h>

#define NOSEGS 0
#define NOSYMS 1
#define NORELS 2

#define NUM_SEGS_PER_FILE 3
#define MAX_NUM_SEGS NUM_SEGS_PER_FILE
#define MAX_NUM_SYMS 64
#define MAX_NUM_RELS (MAX_NUM_SEGS + MAX_NUM_SYMS)

#define INPUT_BUFFER_SIZE 1024
#define OUTPUT_BUFFER_SIZE 1024
#define SEGMENT_BUFFER_SIZE 1024
#define SYMS_BUFFER_SIZE 1024
#define RELS_BUFFER_SIZE 1024
#define DATA_SIZE 1024

/*
 * Linked List to store the segments associated with all input files
 *
 * One node stores all information about a file. The name double array
 * stores the names of all the segments. The rest of the variables store
 * the corresponding data for the segment using the same index.
 *
 * For example, segment indexed 0 has size stored in size[0] and load
 * address at load_addr[0]
 */

typedef struct segments_list {
	struct segments_list *next;
	char *file_name;
	char **name;		// Segment names per input file
	uint32_t *size;		// Segment sizes per input file
	uint32_t *load_addr;	// Segment load addresses
	uint32_t *data;		// Segments data
} segments_list;

#endif				//      __DS_H
