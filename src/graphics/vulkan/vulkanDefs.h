//
// Created by fabio on 22-05-2025.
//

#ifndef VULKANDEFS_H
#define VULKANDEFS_H
#include <vulkan/vulkan.h>
#include <memory.h>
#include <stdlib.h>
#ifdef VGE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elifdef VGE_PLATFORM_WAYLAND
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include "../../window/vgeWindow.h"
#define TRUE 1
#define FALSE 0

#define NUM_DEVICE_EXTENSIONS 1
#define QUEUE_NUMBER 2
#define SPIRV_MAGIC_CONSTANT 0x07230203
#define SHADER_FILE_MAX_SIZE 8192u

typedef struct PhysicalDeviceProperties {
	float maxAnisotropy;
} physicalDeviceProperties, *pPhysicalDeviceProperties;

typedef struct VulkanContext {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
	pPhysicalDeviceProperties physicalDeviceProperties;
    VkDevice device;
    VkQueue queues[QUEUE_NUMBER];

	VkCommandPool commandPool;
#ifndef nDEBUG
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
} vulkanContext, *pVulkanContext;

typedef struct SwapChainSem {
	VkSemaphore renderFinishedSemaphore;
} swapChainSem;

typedef struct FrameContext {
	uint32_t imageIndex;
	VkCommandBuffer commandBuffer;
	VkSemaphore imageAvailableSemaphore;
	swapChainSem *swapSemaphores;
	VkFence RenderFinishedFence;
} frameContext;

typedef struct VgeVulkanTexture {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	VkSampler sampler;
} vgeVulkanTexture, *pVgeVulkanTexture;

typedef struct VgeDepthBuffer {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
} vgeDepthBuffer, *pVgeDepthBuffer;

typedef struct VulkanSwapchain {
    VkSurfaceKHR surface;

	VkSwapchainKHR swapchain;
	VkFormat colorFormat;
	VkExtent2D swapChainExtent;
	uint32_t swapChainImageCount;
	VkImageView *swapChainImageViews;
	VkImage *swapChainImages;
	VkFramebuffer *frameBuffers;
	uint32_t currentFrame;
	frameContext *frameContext;

	pVgeDepthBuffer depth;

	VkClearColorValue clearColor;

	VkRenderPass renderPass;
	pVgeWindow window;
} vulkanSwapchain, *pVulkanSwapchain;

#endif //VULKANDEFS_H
