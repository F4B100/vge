//
// Created by fabio on 8/6/2025.
//

#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <cglm/cglm.h>

typedef struct ObjModel {
	char *name;
	uint32_t numVertex;
	vec3 *vertex;
	uint32_t numTexture;
	vec2 *texture;
	uint32_t numNormal;
	vec3 *normal;
} objModel, *pObjModel;

typedef struct ParserContext {
	objModel model;
} parserContext;

pObjModel parseObjFile(char * filename);

#endif //MODELPARSER_H
