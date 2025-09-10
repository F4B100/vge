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

typedef struct ObjModelInfo {
	pVgeVector vertices;
	pVgeVector textures;
	pVgeVector normals;
	pVgeVector faces;
	pVgeVector indices;
} objModelInfo, *pObjModelInfo;

typedef struct ObjOffsets {
	char *name;
	char *mtlPath;
	uint64_t vertexOffset;
	uint64_t indexOffset;
} objOffsets, *pObjOffsets;

typedef struct ParserContext {
	objModelInfo model;
	pVgeVector offsets;
	pVgeVector faceIndices;
	uint32_t currentIndex;
} parserContext;

typedef struct ObjModel {
	char *name;
	pVgeModel model;
} objModel, *pObjModel;

pVgeVector parseObjFile(pVulkanContext context, pVgePipelineGraphics pipeline, char * filename);

#endif //MODELPARSER_H
