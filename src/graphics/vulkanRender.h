//
// Created by fabio on 20-06-2025.
//

#ifndef VULKANRENDER_H
#define VULKANRENDER_H

#include "vulkanDefs.h"
#include "vulkanCommands.h"

typedef struct FrameContext {
	uint32_t imageIndex;
	VkCommandBuffer commandBuffer;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence RenderFinishedFence;
} frameContext;

frameContext *createFrameContext(uint32_t numberFrame, VkDevice device, VkCommandPool commandPool);
void destroyFrameContext(frameContext *frameContext, VkDevice device, uint32_t numberFrames);

void startFrame(frameContext *frameContext, VkDevice device, VkSwapchainKHR swapChain, VkImage *swapChainImages);
void endFrame(frameContext *frameContext, VkSwapchainKHR swapChain, VkQueue presentQueue, VkQueue graphicsQueue);

#endif //VULKANRENDER_H
