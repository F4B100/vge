//
// Created by fabio on 22-05-2025.
//

#include "vulkanLogicalDevice.h"
#include <stdio.h>

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

	VkPhysicalDevice8BitStorageFeatures device8BitStorageFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR,
    	.pNext = nullptr,
    	.uniformAndStorageBuffer8BitAccess = VK_TRUE,
    	.storageBuffer8BitAccess = VK_TRUE,
    	.storagePushConstant8 = VK_TRUE
	};

	VkPhysicalDeviceBufferDeviceAddressFeatures deviceAddressFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
    	.pNext = &device8BitStorageFeatures,
    	.bufferDeviceAddress = VK_TRUE,
    	.bufferDeviceAddressCaptureReplay = VK_FALSE,
    	.bufferDeviceAddressMultiDevice = VK_FALSE
	};

	VkPhysicalDeviceVulkanMemoryModelFeatures modelFeatures = {
    	.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_MEMORY_MODEL_FEATURES,
    	.pNext = &deviceAddressFeatures,
    	.vulkanMemoryModel = VK_TRUE,
    	.vulkanMemoryModelDeviceScope = VK_TRUE,
    	.vulkanMemoryModelAvailabilityVisibilityChains = VK_FALSE
	};

	VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeatures = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
    	.pNext = &modelFeatures,
    	.timelineSemaphore = VK_TRUE
	};

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &timelineFeatures,
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
