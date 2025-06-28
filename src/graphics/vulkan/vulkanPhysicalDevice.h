//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANPHYSICALDEVICE_H
#define VULKANPHYSICALDEVICE_H
#include "vulkanDefs.h"

void choosePhysicalDevice(vulkanContext *context);
uint8_t isPhysicalDeviceSuitable(vulkanContext *context, VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *physicalDeviceProperties);
uint8_t checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

#endif //VULKANPHYSICALDEVICE_H
