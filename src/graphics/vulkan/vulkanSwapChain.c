//
// Created by fabio on 23-05-2025.
//

#include "vulkanSwapChain.h"

#include "vulkanQueues.h"

swapChainSupportDetails* createSwapChainSupportDetails(const uint32_t sizeFormats, const uint32_t sizeModes) {
    swapChainSupportDetails* details = malloc(sizeof(swapChainSupportDetails));

    if (!details){
        return nullptr;
    }

    details->formatCount = sizeFormats;
    details->formats = malloc(sizeof(VkSurfaceFormatKHR) * sizeFormats);
    if (!details->formats) {
        free(details);
        return nullptr;
    }

    details->presentModeCount = sizeModes;
    details->presentModes = malloc(sizeof(VkPresentModeKHR) * sizeModes);
    if (!details->presentModes) {
        free(details->formats);
        free(details);
        return nullptr;
    }

    return details;
}

void freeSwapChainSupportDetails(swapChainSupportDetails* details) {
    free(details->formats);
    free(details->presentModes);
    free(details);
}

swapChainSupportDetails* querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    uint32_t formatCount, presentModeCount;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    swapChainSupportDetails *supportDetails = createSwapChainSupportDetails(formatCount, presentModeCount);

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &supportDetails->capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, supportDetails->formats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, supportDetails->presentModes);

    return supportDetails;
}

uint32_t chooseSwapSurfaceFormat(swapChainSupportDetails *supportDetails) {
    uint32_t numFormats = supportDetails->formatCount;
    for (int i = 0; i < numFormats; i++) {
        VkSurfaceFormatKHR format = supportDetails->formats[i];
        if (format.format == VK_FORMAT_R8G8B8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return i;
        }
    }
    return -1;
}

uint32_t chooseSwapPresentMode(swapChainSupportDetails *supportDetails) {
    uint32_t numFormats = supportDetails->presentModeCount;
    for (int i = 0; i < numFormats; i++) {
        VkPresentModeKHR presentMode = supportDetails->presentModes[i];
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return i;
        }
    }
    return -1;
}

uint32_t clamp(uint32_t val, uint32_t min, uint32_t max)
{
    if (val > max){
        return max;
    }
    if (val < min) {
        return min;
    }
    return val;
}

VkExtent2D chooseSwapExtent(swapChainSupportDetails *supportDetails, pVgeWindow window) {
    VkSurfaceCapabilitiesKHR *capabilities = &supportDetails->capabilities;
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    }
    uint32_t windowX, windowY;
    vgeGetContentSize(window, &windowX, &windowY);

    VkExtent2D actualExtent = {
        .width = clamp(windowX, capabilities->minImageExtent.width, capabilities->maxImageExtent.width),
        .height = clamp(windowY, capabilities->minImageExtent.height, capabilities->maxImageExtent.height)
    };

    return actualExtent;
}

void createSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, pVgeWindow window, VkExtent2D *swapExtent, VkSwapchainKHR *toCreate) {

    swapChainSupportDetails *details = querySwapChainSupport(physicalDevice, surface);

    uint32_t surfaceFormatIndex = chooseSwapSurfaceFormat(details);
    uint32_t presentModeIndex = chooseSwapPresentMode(details);
    VkSurfaceFormatKHR * surfaceFormat = details->formats + surfaceFormatIndex;
    VkPresentModeKHR * presentMode = details->presentModes + presentModeIndex;
    VkExtent2D extent = chooseSwapExtent(details, window);

    *swapExtent = extent;

    uint32_t imageCount = details->capabilities.minImageCount + 1;

    if (details->capabilities.maxImageCount > 0 && imageCount > details->capabilities.maxImageCount) {
        imageCount = details->capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat->format,
        .imageColorSpace = surfaceFormat->colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = details->capabilities.currentTransform,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = details->capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = *presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    vkCreateSwapchainKHR(device, &createInfo, nullptr, toCreate);
}

uint32_t getSwapChainImages(VkSwapchainKHR swapchain, VkDevice device, VkImage **toStore) {

    uint32_t swapChainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapChainImageCount, nullptr);
    *toStore = (VkImage *)calloc(swapChainImageCount, sizeof(VkImage));
    vkGetSwapchainImagesKHR(device, swapchain, &swapChainImageCount, *toStore);
    return swapChainImageCount;
}

void createSwapChainImageViews(uint32_t imageCount, VkImage *swapChainImages, VkFormat format, VkDevice device, VkImageView **toCreate) {
    VkImageView *swapChainImageViews = calloc(sizeof(VkImageView), imageCount);

    for (uint32_t i = 0; i < imageCount; i++) {
        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i])) {
            printf("failed to create image views!");
        }
    }
    *toCreate = swapChainImageViews;
}

void createFramebuffers(uint32_t imageCount, VkImageView *imageViews, VkDevice device, VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer** toCreate) {
    VkFramebuffer *frameBuffers = malloc(imageCount * sizeof(VkFramebuffer));
    for (size_t i = 0; i < imageCount; i++) {
        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = imageViews + i,
            .width = extent.width,
            .height = extent.height,
            .layers = 1
        };

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffers[i])) {
            printf("failed to create framebuffer!\n");
        }
    }
    *toCreate = frameBuffers;
}

void cleanupSwapChain(VkFramebuffer *frameBuffers, VkImageView *imageViews, VkImage *swapChainImages, VkSwapchainKHR swapChain, uint32_t swapChainImageCount, VkDevice device) {
	vkDeviceWaitIdle(device);
	for (size_t i = 0; i < swapChainImageCount; i++) {
        vkDestroyFramebuffer(device, frameBuffers[i], nullptr);
    }
    free(frameBuffers);

    for (size_t i = 0; i < swapChainImageCount; i++) {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }

    free(imageViews);
    free(swapChainImages);

    vkDestroySwapchainKHR(device, swapChain, nullptr);
}

/**
 * WARNING: only call this after cleanupSwapChain();
 * or else memory leaks will happen and the
 * vulkan validation layers will scream
 */
void createFullSwapChain(VkRenderPass renderPass, VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, pVgeWindow window, VkExtent2D *swapExtent, VkSwapchainKHR*toCreate, uint32_t *numImages, VkImage **swapImages, VkFormat swapFormat, VkImageView **swapImageViews, VkFramebuffer**frameBuffers) {
    vkDeviceWaitIdle(device);
    createSwapChain(physicalDevice, device, surface, window, swapExtent, toCreate);
    *numImages = getSwapChainImages(*toCreate, device, swapImages);
    createSwapChainImageViews(*numImages, *swapImages, swapFormat, device, swapImageViews);
    createFramebuffers(*numImages, *swapImageViews, device, renderPass, *swapExtent, frameBuffers);
}
