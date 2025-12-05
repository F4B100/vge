//
// Created by Fabio on 27/10/2025.
//

#include "utils.h"

#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define LINES_PER_CHUNK 1000

char *read_lines(FILE *file, uint64_t *out_len) {
	char line[MAX_LINE_LENGTH];
	uint64_t buffer_size = MAX_LINE_LENGTH * LINES_PER_CHUNK;
	char *buffer = malloc(buffer_size);
	if (!buffer) return NULL;

	uint64_t  pos = 0;
	int lines_read = 0;

	while (lines_read < LINES_PER_CHUNK && fgets(line, sizeof(line), file)) {
		uint64_t len = strlen(line);

		if (pos + len >= buffer_size) {
			buffer_size *= 2;
			char *new_buffer = realloc(buffer, buffer_size);
			if (!new_buffer) {
				free(buffer);
				return NULL;
			}
			buffer = new_buffer;
		}

		memcpy(buffer + pos, line, len);
		pos += len;
		lines_read++;
	}

	// Null-terminate buffer
	if (pos >= buffer_size) {
		// Ensure space for null terminator
		char *new_buffer = realloc(buffer, buffer_size + 1);
		if (!new_buffer) {
			free(new_buffer);
			free(buffer);
			return NULL;
		}
		buffer = new_buffer;
		buffer_size += 1;
	}
	buffer[pos] = '\0';

	if (out_len) *out_len = pos;
	return buffer;
}