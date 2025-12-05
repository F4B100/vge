//
// Created by fabio on 23-05-2025.
//

#include "vulkanSwapChain.h"

#include <stdio.h>
#include <stdlib.h>

#include "vulkanCommands.h"
#include "vulkanTexture.h"
#include "vulkanQueues.h"
#include "vulkanRender.h"

swapChainSupportDetails * createSwapChainSupportDetails(const uint32_t sizeFormats, const uint32_t sizeModes) {
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

VkSwapchainKHR createSwapChain(pVulkanContext context, pVulkanSwapchain swapchain, pVgeWindow window) {

    swapChainSupportDetails *details = querySwapChainSupport(context->physicalDevice, swapchain->surface);

    uint32_t surfaceFormatIndex = chooseSwapSurfaceFormat(details);
    uint32_t presentModeIndex = chooseSwapPresentMode(details);
    VkSurfaceFormatKHR * surfaceFormat = details->formats + surfaceFormatIndex;
    VkPresentModeKHR * presentMode = details->presentModes + presentModeIndex;
    VkExtent2D extent = chooseSwapExtent(details, window);

    swapchain->swapChainExtent = extent;

    uint32_t imageCount = details->capabilities.minImageCount + 1;

    if (details->capabilities.maxImageCount > 0 && imageCount > details->capabilities.maxImageCount) {
        imageCount = details->capabilities.maxImageCount;
    }


    VkSwapchainCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = swapchain->surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat->format,
        .imageColorSpace = surfaceFormat->colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = details->capabilities.currentTransform,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = *presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

	VkSwapchainKHR swap = VK_NULL_HANDLE;
    vkCreateSwapchainKHR(context->device, &createInfo, nullptr, &swap);
	return swap;
}

void getSwapChainImages(pVulkanContext context, pVulkanSwapchain swapchain) {
    vkGetSwapchainImagesKHR(context->device, swapchain->swapchain, &swapchain->swapChainImageCount, nullptr);
    swapchain->swapChainImages = (VkImage *)calloc(swapchain->swapChainImageCount, sizeof(VkImage));
    vkGetSwapchainImagesKHR(context->device, swapchain->swapchain, &swapchain->swapChainImageCount, swapchain->swapChainImages);
}

void createSwapChainImageViews(pVulkanContext context, pVulkanSwapchain swapchain) {
    swapchain->swapChainImageViews = calloc(sizeof(VkImageView), swapchain->swapChainImageCount);

    for (uint32_t i = 0; i < swapchain->swapChainImageCount; i++) {
        VkImageViewCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
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

        if (vkCreateImageView(context->device, &createInfo, nullptr, swapchain->swapChainImageViews + i)) {
            printf("failed to create image views!");
        }
    }
}

void createRenderPass(pVulkanContext context, pVulkanSwapchain swapchain) {
	VkAttachmentDescription attachments[2] = {
			{
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			},
		{
			.flags = 0,
			.format = VK_FORMAT_D32_SFLOAT,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		}
	};

	VkAttachmentReference attachment_ref[2] = {
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			},
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		}
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachment_ref[0],
		.pDepthStencilAttachment = &attachment_ref[1]
	};

	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|
						VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT|
						VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|
						 VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
	};

	VkRenderPassCreateInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	if (vkCreateRenderPass(context->device, &renderPassInfo, nullptr, &swapchain->renderPass) != VK_SUCCESS) {
		printf("failed to create render pass!");
	}
}

void createFramebuffers(pVulkanContext context, pVulkanSwapchain swapchain) {
    swapchain->frameBuffers = malloc(swapchain->swapChainImageCount * sizeof(VkFramebuffer));
    for (size_t i = 0; i < swapchain->swapChainImageCount; i++) {
    	VkImageView attachments[2] = {
    		*(swapchain->swapChainImageViews + i),
			swapchain->depth->imageView
		};

        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = swapchain->renderPass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = swapchain->swapChainExtent.width,
            .height = swapchain->swapChainExtent.height,
            .layers = 1
        };

        if (vkCreateFramebuffer(context->device, &framebufferInfo, nullptr, swapchain->frameBuffers + i)) {
            printf("failed to create framebuffer!\n");
        }
    }
}

void cleanupSwapChain(pVulkanContext context, pVulkanSwapchain swapchain){
	vkDeviceWaitIdle(context->device);
	destroyFrameContext(context, swapchain);
	for (size_t i = 0; i < swapchain->swapChainImageCount; i++) {
        vkDestroyFramebuffer(context->device, swapchain->frameBuffers[i], nullptr);
    }
    free(swapchain->frameBuffers);

    for (size_t i = 0; i < swapchain->swapChainImageCount; i++) {
        vkDestroyImageView(context->device, swapchain->swapChainImageViews[i], nullptr);
    }

    free(swapchain->swapChainImageViews);
    free(swapchain->swapChainImages);

    vkDestroySwapchainKHR(context->device, swapchain->swapchain, nullptr);

	vkDestroyRenderPass(context->device, swapchain->renderPass, nullptr);

	vkDestroySurfaceKHR(context->instance, swapchain->surface, nullptr);

	free(swapchain);
}

void resizeSwapchain(pVulkanContext context, pVulkanSwapchain swapchain) {
	vkDeviceWaitIdle(context->device);
	for (size_t i = 0; i < swapchain->swapChainImageCount; i++) {
		vkDestroyFramebuffer(context->device, swapchain->frameBuffers[i], nullptr);
	}
	free(swapchain->frameBuffers);

	for (size_t i = 0; i < swapchain->swapChainImageCount; i++) {
		vkDestroyImageView(context->device, swapchain->swapChainImageViews[i], nullptr);
	}

	free(swapchain->swapChainImageViews);
	free(swapchain->swapChainImages);

	vkDestroySwapchainKHR(context->device, swapchain->swapchain, nullptr);

	swapchain->swapchain = createSwapChain(context, swapchain, swapchain->window);
	getSwapChainImages(context, swapchain);
	createSwapChainImageViews(context, swapchain);
    createFramebuffers(context, swapchain);
}

/**
 * WARNING: only call this after cleanupSwapChain();
 * or else memory leaks will happen and the
 * vulkan validation layers will scream
 */
pVulkanSwapchain createFullSwapChain(pVulkanContext context, pVgeWindow window) {
    vkDeviceWaitIdle(context->device);
	pVulkanSwapchain swapchain = malloc(sizeof(pVulkanSwapchain));

	swapchain->colorFormat = VK_FORMAT_R8G8B8A8_SRGB;

	vgeCreateVulkanWindowSurface(window, context->instance, &swapchain->surface);
    swapchain->swapchain = createSwapChain(context, swapchain, window);
    getSwapChainImages(context, swapchain);
    createSwapChainImageViews(context, swapchain);

	int32_t width = (int32_t)swapchain->swapChainExtent.width;
	int32_t height = (int32_t)swapchain->swapChainExtent.height;

	swapchain->depth = createDepthBuffer(context, width, height);

	createRenderPass(context, swapchain);
    createFramebuffers(context, swapchain);
	swapchain->currentFrame = 0;
	swapchain->frameContext = createFrameContext(context, swapchain);

	swapchain->clearColor.float32[0] = 0.0f;
	swapchain->clearColor.float32[1] = 0.0f;
	swapchain->clearColor.float32[2] = 0.0f;
	swapchain->clearColor.float32[3] = 1.0f;

	return swapchain;
}

void setSwapClearValue(pVulkanSwapchain swapchain, vec4 clearColor) {
	memcpy(swapchain->clearColor.float32, &clearColor, sizeof(vec4));
}
