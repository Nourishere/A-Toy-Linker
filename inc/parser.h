/*
 * Parse input text object file and write to output text file
 */

#ifndef __PARSER_H
#define __PARSER_H

#include <stdio.h>

void initialize_buffers(void);
void free_buffers(void);
void process_magic(FILE * input);
int *process_metadata(FILE * input, FILE * output);
void process_segments(int *meta, FILE * input, FILE * output);
void process_symbols(int *meta, FILE * input, FILE * output);
void process_rels(int *meta, FILE * input, FILE * output);
void process_data(int *meta, FILE * input, FILE * output);
void process_line(FILE * input, char *buf);
int index_buffer(int token_number, char *buf);
void zero_input_buffer(void);
void tokenize(char *buf);

#endif				// __PARSER_H
