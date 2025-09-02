//
// Created by fabio on 8/6/2025.
//

#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <cglm/cglm.h>
#include "../../utils/vector/vgeVector.h"

typedef struct ObjFace {
	vec3 faceVertex;
	vec3 faceNormal;
	vec2 faceTexture;
} objFace, *pObjFace;

typedef struct ObjModel {
	char *name;
	char *mtlPath;
	pVgeVector vertices;
	pVgeVector textures;
	pVgeVector normals;
	pVgeVector faces;
	pVgeVector indices;
} objModel, *pObjModel;

typedef struct ParserContext {
	pObjModel model;
} parserContext;

pObjModel parseObjFile(char * filename);

#endif //MODELPARSER_H
