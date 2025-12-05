//
// Created by fabio on 8/6/2025.
//

#ifndef MODELPARSER_H
#define MODELPARSER_H
#pragma once

#include <cglm/cglm.h>
#include "../../graphics/model/modelStructs.h"
#include "../../dataStructs/typeDefs.h"

typedef struct ObjFace {
	vec3 faceVertex;
	vec3 faceNormal;
	vec2 faceTexture;
} objFace, *pObjFace;

typedef struct ObjModelInfo {
	vec3 *vertices;
	vec2 *textures;
	vec3 *normals;
	pObjFace faces;
	pUint32 indices;
} objModelInfo, *pObjModelInfo;

typedef struct ObjOffsets {
	char *name;
	char *mtlPath;
	uint64_t vertexOffset;
	uint64_t indexOffset;
} objOffsets, *pObjOffsets;

typedef struct ParserContext {
	objModelInfo model;
	pObjOffsets offsets;
	ivec3 *faceIndices;
	uint32_t currentIndex;
} parserContext;

typedef struct ObjModel {
	char *name;
	pVgeModel model;
} objModel, *pObjModel;

pObjModel parseObjFile(pVulkanContext context, pVgePipelineGraphics pipeline, char * filepath);

#endif //MODELPARSER_H
