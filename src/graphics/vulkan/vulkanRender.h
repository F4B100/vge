//
// Created by fabio on 20-06-2025.
//

#ifndef VULKANRENDER_H
#define VULKANRENDER_H
#include <vulkan/vulkan.h>
#include "vulkanDefs.h"
#include "../model/modelStructs.h"
#include "../model/modelDescriptors.h"
#include "vulkanPipeline.h"

frameContext *createFrameContext(pVulkanContext context, pVulkanSwapchain swapchain);
void destroyFrameContext(pVulkanContext context, pVulkanSwapchain swapchain);

void startFrame(pVulkanContext context, pVulkanSwapchain swapchain);

void renderPassStart(pVulkanContext context, pVulkanSwapchain swapchain);
void renderPassEnd(pVulkanSwapchain swapchain);

void drawModel(pVulkanContext context, pVulkanSwapchain swapchain, pVgePipelineGraphics graphicsPipeline, pVgeModel model, pVgeDescriptor descriptor);

void endFrame(pVulkanContext context, pVulkanSwapchain swapchain);

#endif //VULKANRENDER_H
