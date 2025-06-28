//
// Created by fabio on 22-05-2025.
//

#include "vulkanQueues.h"
#include <stdio.h>

queueFamilyIndices *searchQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {

    queueFamilyIndices *queueFamilies = calloc(sizeof(queueFamilyIndices), 1);

    if (queueFamilies == nullptr) {
        printf("could not allocate memory for queueFamilyIndices\n");
        return nullptr;
    }

    uint32_t queueFamiliesToAllocate = QUEUE_NUMBER;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    VkQueueFamilyProperties queueFamilyPropertiesArr[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyPropertiesArr);

    for (int i = 0; i < queueFamilyCount; i++) {
        uint32_t thisQueueFamiliesAllocated = 0;
        if (queueFamilyPropertiesArr[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && !queueFamilies->queueInfoArr[0].hasQueueFamily) {
            queueFamilies->queueInfoArr[0].queueFamilyIndex = i;
            queueFamilies->queueInfoArr[0].queuePriority = 1.0f;
            queueFamilies->queueInfoArr[0].hasQueueFamily = TRUE;
            thisQueueFamiliesAllocated += 1;
            queueFamiliesToAllocate -= 1;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport && !queueFamilies->queueInfoArr[1].hasQueueFamily && !queueFamilyPropertiesArr[i].queueCount - thisQueueFamiliesAllocated <= 0) {
            queueFamilies->queueInfoArr[1].queueFamilyIndex = i;
            queueFamilies->queueInfoArr[1].queuePriority = 1.0f;
            queueFamilies->queueInfoArr[1].hasQueueFamily = TRUE;
            thisQueueFamiliesAllocated += 1;
            queueFamiliesToAllocate -= 1;
        }

        if (!queueFamiliesToAllocate) {
            return queueFamilies;
        }
    }
    freeQueueFamilies(queueFamilies);
    return nullptr;
}

void freeQueueFamilies(queueFamilyIndices *queueFamilyIndices) {
    free(queueFamilyIndices);
}

uint8_t isQueueFamiliesComplete(queueFamilyIndices *queueFamilyIndices) {
    uint8_t isComplete = 1;
    for (int i = 0; i < QUEUE_NUMBER; i++) {
        isComplete = isComplete && queueFamilyIndices->queueInfoArr[i].hasQueueFamily;
    }
    return isComplete;
}

