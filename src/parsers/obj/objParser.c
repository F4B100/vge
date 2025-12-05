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

#include "../../dataStructs/list.h"
#include "../utils/utils.h"

pObjModel parseObjFile(pVulkanContext context, pVgePipelineGraphics pipeline, char * filepath) {
	FILE *file = fopen(filepath, "r");
	if (file == NULL) {
		printf("Error opening file\n");
		return nullptr;
	}

	yyscan_t scanner = {};

	if (objlex_init(&scanner)) {
		printf("Error initializing parser for file: %s\n", filepath);
		fclose(file);
		return nullptr;
	}
	objset_in(file, scanner);

	parserContext parserContext = {};

	parserContext.offsets = ListAlloc(sizeof(objOffsets), 0);
	parserContext.faceIndices = ListAlloc(sizeof(ivec3), 0);
	parserContext.model.vertices = ListAlloc(sizeof(vec3), 0);
	parserContext.model.textures = ListAlloc(sizeof(vec2), 0);
	parserContext.model.normals = ListAlloc(sizeof(vec3), 0);
	parserContext.model.faces = ListAlloc(sizeof(objFace), 0);
	parserContext.model.indices = ListAlloc(sizeof(uint32), 0);;

	uint64_t buffLen = 0;
	while (true) {
		char *buff = read_lines(file, &buffLen);
		if (buff == NULL || buffLen == 0) {
			break;
		}
		obj_scan_bytes(buff, (int)buffLen, scanner);
		objparse(&parserContext, scanner);
		free(buff);
	}

	int result = objparse(&parserContext, scanner);

	if (result) {
		printf("Error parsing for file: %s\n", filepath);
	}

	objlex_destroy(scanner);
	fclose(file);

	if (result != 0) {
		fprintf(stderr, "Parse failed.\n");
		return nullptr;
	}

	pObjModel toReturn = ListAlloc(sizeof(objModel), 0);
	uint64_t size = listGetNumElems(parserContext.offsets);
	listExtend(toReturn, size);
	for (uint64_t i = 0; i < size - 1; i++) {
		pObjModel model = &listGetLast(model);
		model->name = ((pObjOffsets)&parserContext.model.faces[i])->name;
		pObjOffsets offsets= &parserContext.offsets[i + 1];

		vgeModelCreateInfo info = {};

		info.pipeline = pipeline;

		vgeVertexInfo vertexInfo = {
			.numVertices = offsets->vertexOffset,
			.data = parserContext.model.faces,
			.sizeVertex = sizeof(objFace)
		};

		info.vertexInfo = &vertexInfo;

		vgeIndexInfo indexInfo = {
			.numIndexes = offsets->indexOffset,
			.data = parserContext.model.indices,
			.sizeIndex = sizeof(uint32)
		};

		info.indexInfo = &indexInfo;

		model->model = createVgeModel(context, &info);
	}
	pObjModel model = &listGetLast(model);

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
