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

 */
static char **segments;
