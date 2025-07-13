//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H
#include <stdint.h>
#include "../vulkan/vulkanDefs.h"

typedef struct VgeVertexInfo {
	uint32_t offset;
	uint32_t size;
	uint32_t stride;
} vgeVertexInfo, *pVgeVertexInfo;

#ifdef VGE_GRAPHICS_VULKAN
#include "../vulkan/vulkanBuffer.h"
#endif //VGE_GRAPHICS_VULKAN


typedef struct VgeModel {
	vgeVertexBuffer vertexBuffer;
	vgeIndexBuffer indexBuffer;
} vgeModel, *pVgeModel;

pVgeModel createVgeModel(void *vertexData, uint32_t *indexData);

#endif //MODELSTRUCTS_H
