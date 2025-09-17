//
// Created by fabio on 7/13/2025.
//

#include "vulkanBuffer.h"

#include <string.h>

pVulkanBuffer initBuffer(vulkanContext *context, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	pVulkanBuffer buffer = malloc(sizeof(vulkanBuffer));

	buffer->size = size;

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(context->device, &bufferInfo, nullptr, &buffer->buffer) != VK_SUCCESS) {
        printf("failed to create buffer!");
    	free(buffer);
    	return nullptr;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(context->device, buffer->buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties)
    };

    if (vkAllocateMemory(context->device, &allocInfo, nullptr, &buffer->bufferMemory) != VK_SUCCESS) {
        printf("failed to allocate buffer memory!");
    	vkDestroyBuffer(context->device, buffer->buffer, nullptr);
    	free(buffer);
    	return nullptr;
    }

    vkBindBufferMemory(context->device, buffer->buffer, buffer->bufferMemory, 0);
	return buffer;
}

void copyBuffer(vulkanContext *context, pVulkanBuffer src, pVulkanBuffer dst) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommand(context);

    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = src->size
    };

    vkCmdCopyBuffer(commandBuffer, src->buffer, dst->buffer, 1, &copyRegion);

    endSingleTimeCommand(context, commandBuffer, context->queues[0]);
}

uint32_t findMemoryType(vulkanContext *context, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(context->physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	printf("failed to find suitable memory type!\n");
	return -1;
}

void destroyBuffer(pVulkanContext context, pVulkanBuffer buffer) {
	if (buffer) {
		if (buffer->buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(context->device, buffer->buffer, nullptr);
			buffer->buffer = VK_NULL_HANDLE;
		}
		if (buffer->bufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(context->device, buffer->bufferMemory, nullptr);
			buffer->bufferMemory = VK_NULL_HANDLE;
		}

		free(buffer);
	}
}

pVulkanBuffer createStagingBuffer(vulkanContext *context, uint32_t numElements, uint32_t sizeElements, void * data) {
	pVulkanBuffer stagingBuffer = initBuffer(
		context,
		sizeElements * numElements,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

	void* deviceData;
	vkMapMemory(context->device, stagingBuffer->bufferMemory, 0, stagingBuffer->size, 0, &deviceData);
	memcpy(deviceData, data, stagingBuffer->size);
	vkUnmapMemory(context->device, stagingBuffer->bufferMemory);
	return stagingBuffer;
}

pVulkanBuffer createVulkanBuffer(vulkanContext *context, uint32_t numElements, uint32_t sizeElements, void * data, VkBufferUsageFlags usage) {
    pVulkanBuffer stagingBuffer = createStagingBuffer(context, numElements, sizeElements, data);

	pVulkanBuffer buffer = initBuffer(
		context,
		sizeElements * numElements,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

    copyBuffer(context, stagingBuffer, buffer);

    destroyBuffer(context, stagingBuffer);
	return buffer;
}

pVulkanBuffer createUniformBuffer(vulkanContext *context, uint64_t sizeUniform, void *data) {
	pVulkanBuffer uniformBuffer = initBuffer(
		context,
		sizeUniform,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	void* mappedData;
	vkMapMemory(context->device, uniformBuffer->bufferMemory, 0, uniformBuffer->size, 0, &mappedData);
	memcpy(mappedData, data, sizeUniform);
	vkUnmapMemory(context->device, uniformBuffer->bufferMemory);

	return uniformBuffer;

}

void updateUniformBuffer(vulkanContext *context, pVulkanBuffer uniformBuffer, uint64_t offset, uint64_t size, void* data) {
	void* mappedData;
	vkMapMemory(context->device, uniformBuffer->bufferMemory, offset, size, 0, &mappedData);
	memcpy(mappedData, data, uniformBuffer->size);
	vkUnmapMemory(context->device, uniformBuffer->bufferMemory);
}
