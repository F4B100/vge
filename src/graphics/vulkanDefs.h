//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANDEFS_H
#define VULKANDEFS_H

#include <vulkan/vulkan.h>
#include "../window/vgeWindow.h"

#define TRUE 1
#define FALSE 0
#define NULL 0

#define NUM_DEVICE_EXTENSIONS 1
#define QUEUE_NUMBER 2

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
typedef struct VkQueueFamilyIndices {
    queueInfo queueInfoArr[QUEUE_NUMBER];
} queueFamilyIndices;

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    VkSurfaceFormatKHR *formats;

    uint32_t presentModeCount;
    VkPresentModeKHR *presentModes;
} swapChainSupportDetails;

typedef struct VulkanContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkDevice device;

    VkSwapchainKHR swapchain;
    uint32_t swapChainImageCount;
    VkImage *swapChainImages;

    vgeWindow *window;
} vulkanContext;

#endif //VULKANDEFS_H
