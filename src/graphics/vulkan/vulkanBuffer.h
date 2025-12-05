//
// Created by fabio on 7/13/2025.
//

#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

#include "vulkanDefs.h"
#include "../vulkan/vulkanCommands.h"

typedef struct VgeVulkanBuffer {
	VkDeviceSize size;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
} vulkanBuffer, *pVulkanBuffer;

typedef struct VgeVertexBuffer {
	void* VertexData;
	vulkanBuffer buffer;
} vgeVertexBuffer, *pVgeVertexBuffer;

typedef struct VgeIndexBuffer {
	uint64_t size;
	uint32_t* indexData;
	vulkanBuffer buffer;
} vgeIndexBuffer, *pVgeIndexBuffer;

pVulkanBuffer initBuffer(pVulkanContext context, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void destroyBuffer(pVulkanContext context, pVulkanBuffer buffer);

void copyBuffer(vulkanContext *context, pVulkanBuffer src, pVulkanBuffer dst) ;

uint32_t findMemoryType(vulkanContext *context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

pVulkanBuffer createStagingBuffer(vulkanContext *context, uint32_t numElements, uint32_t sizeElements, void * data);
pVulkanBuffer createVulkanBuffer(vulkanContext *context, uint32_t numElements, uint32_t sizeElements, void * data, VkBufferUsageFlags usage);

#define createVulkanIndexBuffer(context,numIndexes,sizeIndex,indexData) createVulkanBuffer(context,numIndexes,sizeIndex,indexData,VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
#define createVulkanVertexBuffer(context,numVertexes,sizeVertex,vertexData) createVulkanBuffer(context,numVertexes,sizeVertex,vertexData,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)

#define createVulkanVertexBufferFromInfo(context,vertexInfo) createVulkanVertexBuffer(context,vertexInfo.numVertices,vertexInfo.sizeVertex,vertexInfo.data)
#define createVulkanIndexBufferFromInfo(context,indexInfo) createVulkanIndexBuffer(context,indexInfo.numIndexes,indexInfo.sizeIndex,indexInfo.data)

pVulkanBuffer createUniformBuffer(vulkanContext *context, uint64_t sizeUniform, void * data);
#define createVulkanUniformBufferFromInfo(context,uniformInfo) createUniformBuffer(context,uniformInfo->sizeUniform,uniformInfo->data)

void updateUniformBuffer(vulkanContext *context, pVulkanBuffer uniformBuffer, uint64_t offset, uint64_t size, void* data);

#endif //VULKANBUFFER_H
