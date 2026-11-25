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

#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include <stdio.h>

int initialize_segments_list(void);
void set_file_allocation_data(int *meta, char *file_name);
int clean_segments_list(void);
void allocate(FILE * obj);

#endif				//      __ALLOCATOR_H
