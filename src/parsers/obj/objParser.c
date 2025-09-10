//
// Created by fabio on 8/6/2025.
//

#include "objParser.h"
#include "objParser.tab.h"
#define YYSTYPE OBJSTYPE
#include "objLexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define LINES_PER_CHUNK 1000

char *read_1000_lines(FILE *file, uint64_t *out_len) {
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

pVgeVector parseObjFile(pVulkanContext context, pVgePipelineGraphics pipeline, char * filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening file\n");
		return nullptr;
	}

	yyscan_t scanner = {};

	if (objlex_init(&scanner)) {
		printf("Error initializing parser for file: %s\n", filename);
		fclose(file);
		return nullptr;
	}
	objset_in(file, scanner);

	parserContext parserContext = {};

    parserContext.offsets = vgeVectorInit(sizeof(objOffsets));
    parserContext.faceIndices = vgeVectorInit(sizeof(ivec3));
	parserContext.model.vertices = vgeVectorInit(sizeof(vec3));
	parserContext.model.textures = vgeVectorInit(sizeof(vec2));
	parserContext.model.normals = vgeVectorInit(sizeof(vec3));
	parserContext.model.faces = vgeVectorInit(sizeof(objFace));
	parserContext.model.indices = vgeVectorInit(sizeof(uint32_t));

	char *buff = NULL;
	uint64_t buffLen = 0;
	while (true) {
		buff = read_1000_lines(file, &buffLen);
		if (buff == NULL || buffLen == 0) {
			break;
		}
		obj_scan_bytes(buff, buffLen, scanner);
		objparse(&parserContext, scanner);
		free(buff);
	}

	int result = objparse(&parserContext, scanner);

	if (result) {
		printf("Error parsing for file: %s\n", filename);
	}

	objlex_destroy(scanner);
	fclose(file);

	if (result != 0) {
		fprintf(stderr, "Parse failed.\n");
		return NULL;
	}

	pVgeVector toReturn = vgeVectorInit(sizeof(objModel));
	for (uint64_t i = 0; i < vgeVectorGetSize(parserContext.offsets) - 1; i++) {
		pObjModel model = vgeVectorAppendEmpty(toReturn);
		model->name = ((pObjOffsets)vgeVectorGetElement(parserContext.model.faces, i))->name;
		pObjOffsets offsets= vgeVectorGetElement(parserContext.offsets, i + 1);

		vgeModelCreateInfo info = {};

		info.pipeline = pipeline;

		vgeVertexInfo vertexInfo = {
			.numVertices = offsets->vertexOffset,
			.data = vgeVectorGetData(parserContext.model.faces),
			.sizeVertex = vgeVectorGetSizeElement(parserContext.model.faces)
		};

		info.vertexInfo = &vertexInfo;

		vgeIndexInfo indexInfo = {
			.numIndexes = offsets->indexOffset,
			.data = vgeVectorGetData(parserContext.model.indices),
			.sizeIndex = vgeVectorGetSizeElement(parserContext.model.indices)
		};

		info.indexInfo = &indexInfo;

		model->model = createVgeModel(context, &info);
	}
	pObjModel model = vgeVectorAppendEmpty(toReturn);

	model->name = ((pObjOffsets)vgeVectorGetLast(parserContext.model.faces))->name;

	vgeModelCreateInfo info = {};

	info.pipeline = pipeline;

	vgeVertexInfo vertexInfo = {
		.numVertices = parserContext.model.faces->numElements,
		.data = vgeVectorGetData(parserContext.model.faces),
		.sizeVertex = vgeVectorGetSizeElement(parserContext.model.faces)
	};

	info.vertexInfo = &vertexInfo;

	vgeIndexInfo indexInfo = {
		.numIndexes = parserContext.model.indices->numElements,
		.data = vgeVectorGetData(parserContext.model.indices),
		.sizeIndex = vgeVectorGetSizeElement(parserContext.model.indices)
	};

	info.indexInfo = &indexInfo;

	model->model = createVgeModel(context, &info);

	vgeVectorFree(parserContext.model.vertices);
	vgeVectorFree(parserContext.model.textures);
	vgeVectorFree(parserContext.model.normals);
	vgeVectorFree(parserContext.model.faces);
	vgeVectorFree(parserContext.model.indices);
	vgeVectorFree(parserContext.offsets);
	vgeVectorFree(parserContext.faceIndices);

	return toReturn;
}
