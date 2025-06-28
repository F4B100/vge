//
// Created by fabio on 20-06-2025.
//

#ifndef VULKANCOMMANDS_H
#define VULKANCOMMANDS_H

#include "vulkanDefs.h"

void createCommandPool(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice device, uint32_t queueFamilyIndex, VkCommandPool *toCreate);
VkCommandBuffer *createCommandBuffer(uint32_t numBuffers, VkCommandPool commandPool, VkDevice device);

#endif //VULKANCOMMANDS_H
