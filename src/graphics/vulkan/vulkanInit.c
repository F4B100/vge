//
// Created by fabio on 22-05-2025.
//

#include <stdio.h>
#include "vulkanInit.h"

#include "vulkanCommands.h"
#include "vulkanInstance.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPipeline.h"
#include "vulkanSwapChain.h"
#include "vulkanQueues.h"

pVulkanContext initVulkan(pVgeWindow window) {
    pVulkanContext context = calloc(sizeof(vulkanContext), 1);
    if (!context) {
        return nullptr;
    }

    context->window = window;

	char *windowName;

	vgeGetWindowName(window, &windowName);

    createVulkanInstance(
        windowName,
        context
    );

    printf("after!\n");

    vgeCreateVulkanWindowSurface(
        window,
        context->instance,
        &context->surface
    );

    choosePhysicalDevice(context);

    queueFamilyIndices *queueIndices = searchQueueFamilies(
        context->physicalDevice,
        context->surface
    );

    createLogicalDevice(
        context->physicalDevice,
        queueIndices,
        QUEUE_NUMBER,
        &context->device
    );

    createQueues(
        context->queues,
        queueIndices,
        QUEUE_NUMBER,
        context->device
    );

    createSwapChain(
        context->physicalDevice,
        context->device,
        context->surface,
        context->window,
        &context->swapChainExtent,
        &context->swapchain
    );

    context->swapChainImageCount = getSwapChainImages(
        context->swapchain,
        context->device,
        &context->swapChainImages
    );

    createSwapChainImageViews(
        context->swapChainImageCount,
        context->swapChainImages,
        VK_FORMAT_R8G8B8A8_SRGB,
        context->device,
        &context->swapChainImageViews
        );

    createRenderPass(
        context->device,
        VK_FORMAT_R8G8B8A8_SRGB,
        &context->renderPass
    );

    createFramebuffers(
        context->swapChainImageCount,
        context->swapChainImageViews,
        context->device,
        context->renderPass,
        context->swapChainExtent,
        &context->frameBuffers
    );

    createCommandPool(context->physicalDevice,
        context->surface,
        context->device,
        queueIndices->queueInfoArr[0].queueFamilyIndex,
        &context->commandPool
    );

    return context;
}

void destroyVulkan(pVulkanContext context) {
    vkDeviceWaitIdle(context->device);

    #ifndef NDEBUG
        DestroyDebugUtilsMessengerEXT(context->instance, context->debugMessenger, nullptr);
    #endif

    cleanupSwapChain(
        context->frameBuffers,
        context->swapChainImageViews,
        context->swapChainImages,
        context->swapchain,
        context->swapChainImageCount,
        context->device);

    vkDestroyRenderPass(context->device, context->renderPass, nullptr);

    vkDestroyCommandPool(context->device, context->commandPool, nullptr);

    vkDestroyDevice(context->device, nullptr);

    vkDestroySurfaceKHR(context->instance, context->surface, nullptr);
    vkDestroyInstance(context->instance, nullptr);

    free(context);
}