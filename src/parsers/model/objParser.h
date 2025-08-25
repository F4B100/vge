//
// Created by fabio on 8/6/2025.
//

#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <cglm/cglm.h>

typedef struct ObjModel {
	char *name;
	uint64_t numVertex;
	vec3 *vertex;
	uint64_t numTexture;
	vec2 *texture;
	uint64_t numNormal;
	vec3 *normal;
	uint64_t numFaces;
	float *faces;
	uint64_t numIndices;
	uint32_t *indices;
} objModel, *pObjModel;

typedef struct ParserContext {
	pObjModel model;
} parserContext;

pObjModel parseObjFile(char * filename);

#endif //MODELPARSER_H
