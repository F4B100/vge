//
// Created by fabio on 22-05-2025.
//

#include <stdio.h>
#include "vulkanInit.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanLogicalDevice.h"
#include "vulkanSwapChain.h"
#include "vulkanQueues.h"


vulkanContext *initVulkan(vgeWindow *window) {
    vulkanContext * context = calloc(sizeof(vulkanContext), 1);
    if (!context) {
        return nullptr;
    }

    context->window = window;

    createVulkanInstance(context);
    glfwCreateWindowSurface(context->instance, window->window, nullptr, &context->surface);
    choosePhysicalDevice(context);
    queueFamilyIndices *queueIndices = searchQueueFamilies(context->physicalDevice, context->surface);
    createLogicalDevice(context->physicalDevice, queueIndices, QUEUE_NUMBER, &context->device);
    createQueues(context->queues, queueIndices, QUEUE_NUMBER, context->device);
    createSwapChain(context->physicalDevice, context->device, context->surface, context->window, &context->swapchain);
    context->swapChainImageCount = getSwapChainImages(context->swapchain, context->device, &context->swapChainImages);

    context->swapChainImageViews = createSwapChainImageViews(
        context->swapChainImageCount,
        context->swapChainImages,
        VK_FORMAT_R8G8B8A8_SRGB,
        context->device
        );

    return context;
}

void createVulkanInstance(vulkanContext *context) {

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_MAKE_VERSION(1, 4, 0),
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pApplicationName = glfwGetWindowTitle(context->window->window),
        .pEngineName = "vge"
    };

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (int i = 0; i < glfwExtensionCount; ++i) {
        printf("%s\n", glfwExtensions[i]);
    }

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr
    };

    if (vkCreateInstance(&createInfo, nullptr, &context->instance)) {
        fprintf(stderr, "Instance Creation Failed\n");
    }
}

