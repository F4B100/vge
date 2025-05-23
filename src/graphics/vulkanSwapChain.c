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

VkExtent2D chooseSwapExtent(swapChainSupportDetails *supportDetails, vgeWindow *window) {
    VkSurfaceCapabilitiesKHR *capabilities = &supportDetails->capabilities;
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    }
    uint32_t windowX, windowY;
    glfwGetWindowSize(window->window, &windowX, &windowY);

    VkExtent2D actualExtent = {
        .width = clamp(windowX, capabilities->minImageExtent.width, capabilities->maxImageExtent.width),
        .height = clamp(windowY, capabilities->minImageExtent.height, capabilities->maxImageExtent.height)
    };

    return actualExtent;
}

void createSwapChain(vulkanContext *context) {

    swapChainSupportDetails *details = querySwapChainSupport(context->physicalDevice, context->surface);

    uint32_t surfaceFormatIndex = chooseSwapSurfaceFormat(details);
    uint32_t presentModeIndex = chooseSwapPresentMode(details);
    VkSurfaceFormatKHR * surfaceFormat = details->formats + surfaceFormatIndex;
    VkPresentModeKHR * presentMode = details->presentModes + presentModeIndex;
    VkExtent2D extent = chooseSwapExtent(details, context->window);

    uint32_t imageCount = details->capabilities.minImageCount + 1;

    if (details->capabilities.maxImageCount > 0 && imageCount > details->capabilities.maxImageCount) {
        imageCount = details->capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = context->surface,
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

    vkCreateSwapchainKHR(context->device, &createInfo, nullptr, &context->swapchain);
}

void getSwapChainImages(vulkanContext *context) {

    vkGetSwapchainImagesKHR(context->device, context->swapchain, &context->swapChainImageCount, nullptr);
    context->swapChainImages = calloc(sizeof(VkImage), context->swapChainImageCount);

    vkGetSwapchainImagesKHR(context->device, context->swapChainImages, &context->swapChainImages, context->swapChainImages);
}
