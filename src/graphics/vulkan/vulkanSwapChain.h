//
// Created by fabio on 23-05-2025.
//

#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "cglm/call/ivec4.h"
#include "vulkanDefs.h"

typedef struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	uint32_t formatCount;
	VkSurfaceFormatKHR *formats;

	uint32_t presentModeCount;
	VkPresentModeKHR *presentModes;
} swapChainSupportDetails;

pVulkanSwapchain createFullSwapChain(pVulkanContext context, pVgeWindow window);
void resizeSwapchain(pVulkanContext context, pVulkanSwapchain swapchain);
void cleanupSwapChain(pVulkanContext context, pVulkanSwapchain swapchain);

void createRenderPass(pVulkanContext context, pVulkanSwapchain swapchain);

VkSwapchainKHR createSwapChain(pVulkanContext context, pVulkanSwapchain swapchain, pVgeWindow window);

void getSwapChainImages(pVulkanContext context, pVulkanSwapchain swapchain);

void createSwapChainImageViews(pVulkanContext context, pVulkanSwapchain swapchain);

void createFramebuffers(pVulkanContext context, pVulkanSwapchain swapchain);

swapChainSupportDetails* createSwapChainSupportDetails(uint32_t sizeFormats, uint32_t sizeModes);
void freeSwapChainSupportDetails(swapChainSupportDetails* details);
swapChainSupportDetails* querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

void setSwapClearValue(pVulkanSwapchain swapchain, vec4 clearColor);

uint32_t chooseSwapSurfaceFormat(swapChainSupportDetails *supportDetails);
uint32_t chooseSwapPresentMode(swapChainSupportDetails *supportDetails);
uint32_t clamp(uint32_t val, uint32_t min, uint32_t max);
VkExtent2D chooseSwapExtent(swapChainSupportDetails *supportDetails, pVgeWindow window);

#endif //VULKANSWAPCHAIN_H
