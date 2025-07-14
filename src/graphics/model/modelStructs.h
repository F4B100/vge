//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H
#include <stdint.h>

typedef struct VgeVertexInfo {
	uint64_t numVerices;
	uint32_t sizeVertex;
	void * data;
} vgeVertexInfo, *pVgeVertexInfo;

typedef struct VgeIndexInfo {
	uint64_t numIndexes;
	uint32_t sizeIndex;
	void * data;
} vgeIndexInfo, *pVgeIndexInfo;

#ifdef VGE_GRAPHICS_VULKAN
#include "../vulkan/vulkanBuffer.h"
#endif //VGE_GRAPHICS_VULKAN


typedef struct VgeModel {
	vgeVertexInfo vertexInfo;
	pVulkanBuffer vertexBuffer;
	vgeIndexInfo indexInfo;
	pVulkanBuffer indexBuffer;
} vgeModel, *pVgeModel;

pVgeModel createVgeModel(pVulkanContext context, pVgeIndexInfo indexInfo, pVgeVertexInfo vertexInfo);

#endif //MODELSTRUCTS_H
