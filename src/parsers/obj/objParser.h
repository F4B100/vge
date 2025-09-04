//
// Created by fabio on 8/6/2025.
//

#ifndef MODELPARSER_H
#define MODELPARSER_H

#include <cglm/cglm.h>
#include "../../utils/vector/vgeVector.h"
#include "../../graphics/model/modelStructs.h"

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
	pVgeVector faceIndices;
	uint32_t currentIndex;
} parserContext;

pVgeModel parseObjFile(pVulkanContext context, pVgePipelineGraphics pipeline, char * filename);

#endif //MODELPARSER_H
