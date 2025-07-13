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

void createBuffer(vulkanContext *context, pVulkanBuffer buffer, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

void copyBuffer(vulkanContext *context, pVulkanBuffer src, pVulkanBuffer dst, uint64_t size);

uint32_t findMemoryType(vulkanContext *context, uint32_t typeFilter, VkMemoryPropertyFlags properties);


#endif //VULKANBUFFER_H
