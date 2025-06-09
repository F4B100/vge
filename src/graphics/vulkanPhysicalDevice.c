//
// Created by fabio on 22-05-2025.
//

#include "vulkanPhysicalDevice.h"
#include "vulkanQueues.h"
#include "vulkanSwapChain.h"
#include <string.h>

char *deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


uint8_t isPhysicalDeviceSuitable(vulkanContext *context, VkPhysicalDevice physicalDevice, VkPhysicalDeviceProperties *physicalDeviceProperties) {
    queueFamilyIndices* queues = searchQueueFamilies(physicalDevice, context->surface);

    uint8_t isSuitable = isQueueFamiliesComplete(queues);

    isSuitable &= glfwGetPhysicalDevicePresentationSupport(context->instance, physicalDevice, queues->queueInfoArr[0].queueFamilyIndex);

    isSuitable &= checkDeviceExtensionSupport(physicalDevice);

    swapChainSupportDetails *details = querySwapChainSupport(physicalDevice, context->surface);

    isSuitable &= details->presentModeCount != 0 && details->formatCount != 0;

    freeSwapChainSupportDetails(details);

    isSuitable &= physicalDeviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ||
        physicalDeviceProperties->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

    return isSuitable;
}

uint8_t checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;

    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    VkExtensionProperties availableExtensions[extensionCount];
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions);

    uint8_t support[NUM_DEVICE_EXTENSIONS];

    for (int i = 0; i < extensionCount; i++) {
        for (int j = 0; j < NUM_DEVICE_EXTENSIONS; j++) {
            if (!strcmp(availableExtensions[i].extensionName, deviceExtensions[j])) {
                support[j] = 1;
            }
        }
    }

    uint8_t result = 1;

    for (int i = 0; i < NUM_DEVICE_EXTENSIONS; i++) {
        result = result && support[i];
    }

    return result;
}

void choosePhysicalDevice(vulkanContext *context) {
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(context->instance, &physicalDevicesCount, nullptr);

    if (physicalDevicesCount == 0) {
        fprintf(stderr, "No Vulkan-compatible GPUs found.\n");
        return;
    }

    VkPhysicalDevice* physicalDevices = malloc(sizeof(VkPhysicalDevice) * physicalDevicesCount);
    if (!physicalDevices) {
        fprintf(stderr, "Failed to allocate memory for physical devices.\n");
        return;
    }

    vkEnumeratePhysicalDevices(context->instance, &physicalDevicesCount, physicalDevices);

    for (uint32_t i = 0; i < physicalDevicesCount; i++) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

        if (isPhysicalDeviceSuitable(context, physicalDevices[i], &properties)) {
            context->physicalDevice = physicalDevices[i];
            printf("Choose Physical Device: %s\n", properties.deviceName);
        }
    }

    if (context->physicalDevice == VK_NULL_HANDLE) {
        fprintf(stderr, "Failed to find a suitable GPU.\n");
    }

    free(physicalDevices);
}