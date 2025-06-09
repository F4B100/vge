//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANLOGICALDEVICE_H
#define VULKANLOGICALDEVICE_H

#include "vulkanDefs.h"

#define NUM_DEVICE_EXTENSIONS 1

void createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice *toCreate);
void createQueues(VkQueue *queues, queueFamilyIndices *queueIndices, uint32_t numQueues, VkDevice device);

#endif //VULKANLOGICALDEVICE_H
