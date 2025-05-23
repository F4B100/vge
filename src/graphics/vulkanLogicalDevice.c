//
// Created by fabio on 22-05-2025.
//

#include "vulkanLogicalDevice.h"

#include "vulkanQueues.h"

char * logicalDeviceExtensions [] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, VkDevice *toCreate) {
    queueFamilyIndices *queues = searchQueueFamilies(physicalDevice, surface);

    VkDeviceQueueCreateInfo queueCreateInfo[QUEUE_NUMBER];
    for (int i = 0; i < QUEUE_NUMBER; i++) {
        queueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[i].pNext = NULL;
        queueCreateInfo[i].queueCount = 1;
        queueCreateInfo[i].pQueuePriorities = &queues->queueInfoArr[i].queuePriority;
        queueCreateInfo[i].queueFamilyIndex = queues->queueInfoArr[i].queueFamilyIndex;
    }

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 2,
        .pQueueCreateInfos = queueCreateInfo,
        .pEnabledFeatures = {},
        .enabledExtensionCount = NUM_DEVICE_EXTENSIONS,
        .ppEnabledExtensionNames = (const char * const*)logicalDeviceExtensions
    };

    vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, toCreate);
}
