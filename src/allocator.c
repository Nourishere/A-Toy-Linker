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
