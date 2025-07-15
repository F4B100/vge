//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H
#include <stdint.h>
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

typedef struct VgeUniformInfo {
	uint64_t sizeUniform;
	void * data;
} vgeUniformInfo, *pVgeUniformInfo;

typedef struct VgeUniformBuffer {
	pVulkanBuffer uniformBuffer;
	VkDescriptorSet descriptorSet;
} vgeUniformBuffer, *pVgeUniformBuffer;

typedef struct VgeModel {
	pVgeVertexInfo vertexInfo;
	pVulkanBuffer vertexBuffer;
	pVgeIndexInfo indexInfo;
	pVulkanBuffer indexBuffer;
	pVgeUniformInfo uniformInfo;
	pVgeUniformBuffer uniformBuffer;
} vgeModel, *pVgeModel;

pVgeModel createVgeModel(pVulkanContext context, pVgeIndexInfo indexInfo, pVgeVertexInfo vertexInfo, pVgeUniformInfo uniformInfo, pVgePipelineGraphics pipeline);
void destroyVgeModel(pVulkanContext context, pVgeModel model);

#endif //MODELSTRUCTS_H
