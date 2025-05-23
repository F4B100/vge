//
// Created by fabio on 22-05-2025.
//

#include "vulkanInit.h"
#include "vulkanPhysicalDevice.h"
#include <stdio.h>

#include "vulkanLogicalDevice.h"
#include "vulkanSwapChain.h"


vulkanContext *initVulkan(vgeWindow *window) {
    vulkanContext * context = malloc(sizeof(vulkanContext));
    if (!context) {
        return nullptr;
    }

    context->window = window;

    createVulkanInstance(context);
    glfwCreateWindowSurface(context->instance, window->window, nullptr, &context->surface);
    choosePhysicalDevice(context);
    createLogicalDevice(context->physicalDevice, context->surface, &context->device);
    createSwapChain(context);





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

