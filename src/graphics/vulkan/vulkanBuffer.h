//
// Created by fabio on 7/13/2025.
//

#ifndef VULKANBUFFER_H
#define VULKANBUFFER_H

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

pVulkanBuffer initBuffer(vulkanContext *context, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
void destroyBuffer(pVulkanContext context, pVulkanBuffer buffer);

void copyBuffer(vulkanContext *context, pVulkanBuffer src, pVulkanBuffer dst) ;

uint32_t findMemoryType(vulkanContext *context, uint32_t typeFilter, VkMemoryPropertyFlags properties);

pVulkanBuffer createVulkanBuffer(vulkanContext *context, uint32_t numIndexes, uint32_t sizeIndex, void * data, VkBufferUsageFlags usage);

#define createVulkanIndexBuffer(context,numIndexes,sizeIndex,indexData) createVulkanBuffer(context, numIndexes, sizeIndex, indexData, VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
#define createVulkanVertexBuffer(context,numVertexes,sizeVertex,vertexData) createVulkanBuffer(context, numVertexes, sizeVertex, vertexData, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)

#endif //VULKANBUFFER_H
