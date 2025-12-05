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

pVulkanContext initVulkan(char *appName) {
    pVulkanContext context = calloc(sizeof(vulkanContext), 1);
    if (!context) {
        return nullptr;
    }

    createVulkanInstance(
        appName,
        context
    );

    choosePhysicalDevice(context);

    queueFamilyIndices *queueIndices = searchQueueFamilies(
        context->physicalDevice
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

	context->commandPool = createCommandPool(context, 0);

	return context;
}

void destroyVulkan(pVulkanContext context) {
    vkDeviceWaitIdle(context->device);

    #ifndef NDEBUG
        DestroyDebugUtilsMessengerEXT(context->instance, context->debugMessenger, nullptr);
    #endif

    vkDestroyCommandPool(context->device, context->commandPool, nullptr);
    vkDestroyDevice(context->device, nullptr);
    vkDestroyInstance(context->instance, nullptr);

    free(context);
}