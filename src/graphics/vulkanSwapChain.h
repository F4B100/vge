//
// Created by fabio on 23-05-2025.
//

#ifndef VULKANSWAPCHAIN_H
#define VULKANSWAPCHAIN_H

#include "vulkanDefs.h"

typedef struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	uint32_t formatCount;
	VkSurfaceFormatKHR *formats;

	uint32_t presentModeCount;
	VkPresentModeKHR *presentModes;
} swapChainSupportDetails;

void createSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, vgeWindow *window, VkSwapchainKHR *toCreate);

uint32_t getSwapChainImages(VkSwapchainKHR swapchain, VkDevice device, VkImage **toStore);

VkImageView *createSwapChainImageViews(uint32_t imageCount, VkImage *swapChainImages, VkFormat format, VkDevice device);

swapChainSupportDetails* createSwapChainSupportDetails(const uint32_t sizeFormats, const uint32_t sizeModes);
void freeSwapChainSupportDetails(swapChainSupportDetails* details);
swapChainSupportDetails* querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

uint32_t chooseSwapSurfaceFormat(swapChainSupportDetails *supportDetails);
uint32_t chooseSwapPresentMode(swapChainSupportDetails *supportDetails);
uint32_t clamp(uint32_t val, uint32_t min, uint32_t max);
VkExtent2D chooseSwapExtent(swapChainSupportDetails *supportDetails, vgeWindow *window);

#endif //VULKANSWAPCHAIN_H
