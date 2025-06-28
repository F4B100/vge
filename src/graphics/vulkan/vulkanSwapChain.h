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

void createFullSwapChain(VkRenderPass renderPass, VkPhysicalDevice physicalDevice, VkDevice device,
	VkSurfaceKHR surface, vgeWindow *window, VkExtent2D *swapExtent, VkSwapchainKHR*toCreate, uint32_t *numImages,
	VkImage **swapImages, VkFormat swapFormat, VkImageView **swapImageViews, VkFramebuffer**frameBuffers);

void cleanupSwapChain(VkFramebuffer *frameBuffers, VkImageView *imageViews, VkImage *swapChainImages,
	VkSwapchainKHR swapChain, uint32_t swapChainImageCount, VkDevice device);

void createSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, vgeWindow *window, VkExtent2D *swapExtent, VkSwapchainKHR *toCreate);

uint32_t getSwapChainImages(VkSwapchainKHR swapchain, VkDevice device, VkImage **toStore);

void createSwapChainImageViews(uint32_t imageCount, VkImage *swapChainImages, VkFormat format, VkDevice device, VkImageView **toCreate);

void createFramebuffers(uint32_t imageCount, VkImageView *imageViews, VkDevice device, VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer** toCreate) ;

swapChainSupportDetails* createSwapChainSupportDetails(const uint32_t sizeFormats, const uint32_t sizeModes);
void freeSwapChainSupportDetails(swapChainSupportDetails* details);
swapChainSupportDetails* querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

uint32_t chooseSwapSurfaceFormat(swapChainSupportDetails *supportDetails);
uint32_t chooseSwapPresentMode(swapChainSupportDetails *supportDetails);
uint32_t clamp(uint32_t val, uint32_t min, uint32_t max);
VkExtent2D chooseSwapExtent(swapChainSupportDetails *supportDetails, vgeWindow *window);

#endif //VULKANSWAPCHAIN_H
