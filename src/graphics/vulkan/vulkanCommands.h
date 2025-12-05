//
// Created by fabio on 20-06-2025.
//
#ifndef VULKANCOMMANDS_H
#define VULKANCOMMANDS_H
#ifdef VGE_GRAPHICS_VULKAN
#include "vulkanDefs.h"
#include "vulkanQueues.h"
#include <stdio.h>
#include <stdlib.h>

VkCommandPool createCommandPool(pVulkanContext context, uint32_t queueFamilyIndex);
VkCommandBuffer *createCommandBuffer(uint32_t numBuffers, VkCommandPool commandPool, VkDevice device);

VkCommandBuffer beginSingleTimeCommand(pVulkanContext context);

void endSingleTimeCommand(pVulkanContext context, VkCommandBuffer commandBuffer, VkQueue queue);

#endif //VGE_GRAPHICS_VULKAN
#endif //VULKANCOMMANDS_H