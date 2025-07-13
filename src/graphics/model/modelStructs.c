//
// Created by fabio on 6/29/2025.
//

#include "modelStructs.h"
#include "../vulkan/vulkanCommands.h"

void createBuffer(vulkanContext *context, pVulkanBuffer buffer, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(context->device, &bufferInfo, nullptr, &buffer->buffer) != VK_SUCCESS) {
        printf("failed to create buffer!");
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
    }

    vkBindBufferMemory(context->device, buffer->buffer, buffer->bufferMemory, 0);
}

void copyBuffer(vulkanContext *context, pVulkanBuffer src, pVulkanBuffer dst, uint64_t size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommand(context);

    VkBufferCopy copyRegion = {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = size
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

