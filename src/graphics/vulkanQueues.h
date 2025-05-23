//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANQUEUES_H
#define VULKANQUEUES_H

#include "vulkanDefs.h"
#include <stdio.h>

queueFamilyIndices *searchQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

void freeQueueFamilies(queueFamilyIndices *queueFamilyIndices);

uint8_t isQueueFamiliesComplete(queueFamilyIndices *queueFamilyIndices);
void freeSwapChainSupportDetails(swapChainSupportDetails* details);

#endif //VULKANQUEUES_H
