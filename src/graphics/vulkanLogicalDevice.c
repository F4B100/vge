//
// Created by fabio on 22-05-2025.
//

#include "vulkanLogicalDevice.h"

#include "vulkanQueues.h"

char * logicalDeviceExtensions [] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

void createLogicalDevice(VkPhysicalDevice physicalDevice, queueFamilyIndices *queueIndices, uint32_t numQueues, VkDevice *toCreate) {

    VkDeviceQueueCreateInfo *queueCreateInfo = calloc(sizeof(VkDeviceQueueCreateInfo), numQueues);
    for (int i = 0; i < numQueues; i++) {
        queueCreateInfo[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo[i].pNext = nullptr;
        queueCreateInfo[i].queueCount = 1;
        queueCreateInfo[i].pQueuePriorities = &queueIndices->queueInfoArr[i].queuePriority;
        queueCreateInfo[i].queueFamilyIndex = queueIndices->queueInfoArr[i].queueFamilyIndex;
    }

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = 2,
        .pQueueCreateInfos = queueCreateInfo,
        .pEnabledFeatures = nullptr,
        .enabledExtensionCount = NUM_DEVICE_EXTENSIONS,
        .ppEnabledExtensionNames = (const char * const*)logicalDeviceExtensions,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
    };

    VkResult res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, toCreate);

    if (res != VK_SUCCESS) {
        fprintf(stderr, "failed to create logical device\n");
    }
    free(queueCreateInfo);
}

void createQueues(VkQueue *queues, queueFamilyIndices *queueIndices, uint32_t numQueues, VkDevice device) {
    for (int i = 0; i < numQueues; ++i) {
        vkGetDeviceQueue(device, queueIndices->queueInfoArr[i].queueFamilyIndex, 0, queues + i);
    }
}
