//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANQUEUES_H
#define VULKANQUEUES_H

#include "vulkanDefs.h"

typedef struct QueueInfo {
	uint8_t hasQueueFamily;
	uint32_t queueFamilyIndex;
	float queuePriority;
} queueInfo;

/*
 * the following is a list of what queue each index represents:
 * 0 - GraphicsQueue / Transfer Queue
 * 1 - PresentationQueue
 */

#define GRAPHICS_QUEUE 0
#define PRESENTATION_QUEUE 1

typedef struct VkQueueFamilyIndices {
	queueInfo queueInfoArr[QUEUE_NUMBER];
} queueFamilyIndices;


queueFamilyIndices *searchQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
void freeQueueFamilies(queueFamilyIndices *queueFamilyIndices);
uint8_t isQueueFamiliesComplete(queueFamilyIndices *queueFamilyIndices);

#endif //VULKANQUEUES_H
