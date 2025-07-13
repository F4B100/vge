//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANDEFS_H
#define VULKANDEFS_H

#ifdef VGE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elifdef VGE_PLATFORM_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include <vulkan/vulkan.h>
#include "../../window/vgeWindow.h"
#include "cglm/call/vec3.h"

#define TRUE 1
#define FALSE 0

#define NUM_DEVICE_EXTENSIONS 1
#define QUEUE_NUMBER 2
#define SPIRV_MAGIC_CONSTANT 0x07230203
#define SHADER_FILE_MAX_SIZE 8192u

typedef struct VulkanContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;

    VkDevice device;
    VkQueue queues[QUEUE_NUMBER];

    VkSwapchainKHR swapchain;
    VkExtent2D swapChainExtent;
    uint32_t swapChainImageCount;
    VkImageView *swapChainImageViews;
    VkImage *swapChainImages;
    VkFramebuffer *frameBuffers;

    VkRenderPass renderPass;

    VkCommandPool commandPool;

    vgeWindow *window;

#ifndef nDEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
} vulkanContext;

#endif //VULKANDEFS_H
