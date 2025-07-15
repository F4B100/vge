//
// Created by fabio on 20-06-2025.
//

#ifndef VULKANRENDER_H
#define VULKANRENDER_H

#include "vulkanDefs.h"
#include "../model/modelStructs.h"
#include "vulkanPipeline.h"

typedef struct SwapChainSem {
	VkSemaphore renderFinishedSemaphore;
} swapChainSem;

typedef struct FrameContext {
	uint32_t imageIndex;
	VkCommandBuffer commandBuffer;
	VkSemaphore imageAvailableSemaphore;
	swapChainSem *swapSemaphores;
	VkFence RenderFinishedFence;
} frameContext;

frameContext *createFrameContext(uint32_t numberFrame, uint32_t numberImages, VkDevice device, VkCommandPool commandPool) ;
void destroyFrameContext(frameContext *frameContext, VkDevice device, uint32_t numberFrames, uint32_t numberImages);

void startFrame(frameContext *frameContext, VkDevice device, VkSwapchainKHR swapChain, VkImage *swapChainImages);

void renderPassStart(frameContext *frameContext, VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer *frameBuffers, VkClearValue *clearValues);
void renderPassEnd(frameContext *frameContext);

void drawModel(pVulkanContext context, frameContext *frameContext, pVgePipelineGraphics graphicsPipeline, pVgeModel model) ;

void endFrame(frameContext *frameContext, VkSwapchainKHR swapChain, VkQueue presentQueue, VkQueue graphicsQueue);

#endif //VULKANRENDER_H
