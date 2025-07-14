//
// Created by fabio on 20-06-2025.
//
#ifdef VGE_GRAPHICS_VULKAN
#include "vulkanCommands.h"

#include "vulkanQueues.h"

void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device, uint32_t queueFamilyIndex, VkCommandPool *toCreate) {
    queueFamilyIndices *queueFamilyIndices = searchQueueFamilies(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamilyIndex
    };

    if (vkCreateCommandPool(device, &poolInfo, nullptr, toCreate)) {
        printf("failed to create command pool!");
    }
}

VkCommandBuffer *createCommandBuffer(uint32_t numBuffers, VkCommandPool commandPool, VkDevice device) {
    VkCommandBuffer *buffers = calloc(numBuffers, sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = numBuffers
    };

    if (vkAllocateCommandBuffers(device, &allocInfo, buffers) != VK_SUCCESS) {
        printf("failed to allocate command buffers!");
    }
    return buffers;
}

VkCommandBuffer beginSingleTimeCommand(pVulkanContext context) {
	VkCommandBufferAllocateInfo allocInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
											 .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
											 .commandPool = context->commandPool,
											 .commandBufferCount = 1};

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(context->device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
										  .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void endSingleTimeCommand(pVulkanContext context, VkCommandBuffer commandBuffer, VkQueue queue) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(context->device, context->commandPool, 1, &commandBuffer);
}

#endif
