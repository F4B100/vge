//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include "../vulkan/vulkanDefs.h"
#include "../vulkan/vulkanPipeline.h"
#include "../vulkan/vulkanBuffer.h"

typedef struct VgeVertexInfo {
	uint64_t numVertices;
	uint32_t sizeVertex;
	void * data;
} vgeVertexInfo, *pVgeVertexInfo;

typedef struct VgeIndexInfo {
	uint64_t numIndexes;
	uint32_t sizeIndex;
	void * data;
} vgeIndexInfo, *pVgeIndexInfo;

typedef struct VgeModelCreateInfo{
	pVgeVertexInfo vertexInfo;
	pVgeIndexInfo indexInfo;
	pVgePipelineGraphics pipeline;
} vgeModelCreateInfo, *pVgeModelCreateInfo;

typedef struct VgeModel {
	vgeVertexInfo vertexInfo;
	pVulkanBuffer vertexBuffer;
	vgeIndexInfo indexInfo;
	pVulkanBuffer indexBuffer;
} vgeModel, *pVgeModel;

pVgeModel createVgeModel(pVulkanContext context, pVgeModelCreateInfo modelCreateInfo);
void destroyVgeModel(pVulkanContext context, pVgeModel model);

#endif //MODELSTRUCTS_H
