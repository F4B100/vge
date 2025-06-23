//
// Created by fabio on 20-06-2025.
//

#include "vulkanRender.h"

frameContext *createFrameContext(uint32_t numberFrame, VkDevice device, VkCommandPool commandPool) {
	const VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0
	};
	const VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};
	const VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	frameContext *frameContext = calloc(numberFrame, sizeof(struct FrameContext));

	for (int i = 0; i < numberFrame; i++) {
		vkCreateFence(device, &fenceCreateInfo, nullptr, &frameContext[i].RenderFinishedFence);
		printf("Created fence[%d] = %p\n", i, frameContext[i].RenderFinishedFence);
		vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frameContext[i].imageAvailableSemaphore);
		vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frameContext[i].renderFinishedSemaphore);
		vkAllocateCommandBuffers(device, &allocInfo, &frameContext[i].commandBuffer);
	}

	return frameContext;
}
void destroyFrameContext(frameContext *frameContext, VkDevice device, uint32_t numberFrames) {
	vkDeviceWaitIdle(device);
	for (int i = 0; i < numberFrames; i++) {
		vkDestroySemaphore(device, frameContext[i].imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(device, frameContext[i].renderFinishedSemaphore, nullptr);
		vkWaitForFences(device, 1, &frameContext[i].RenderFinishedFence, VK_TRUE, UINT64_MAX);
		vkDestroyFence(device, frameContext[i].RenderFinishedFence, nullptr);
		// Begin command buffer recording
		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0,
			.pInheritanceInfo = nullptr
		};

		if (vkBeginCommandBuffer(frameContext[i].commandBuffer, &beginInfo) != VK_SUCCESS) {
			printf("failed to begin recording command buffer!");
			return;
		}
		vkEndCommandBuffer(frameContext[i].commandBuffer);
	}
}

void startFrame(frameContext *frameContext, VkDevice device, VkSwapchainKHR swapChain, VkImage *swapChainImages) {
	printf("Waiting on fence = %p\n", frameContext->RenderFinishedFence);
	vkWaitForFences(device, 1, &frameContext->RenderFinishedFence, VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, frameContext->imageAvailableSemaphore, VK_NULL_HANDLE, &frameContext->imageIndex);

	vkResetFences(device, 1, &frameContext->RenderFinishedFence);

	vkResetCommandBuffer(frameContext->commandBuffer, 0);

	// Begin command buffer recording
	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = NULL
	};

	if (vkBeginCommandBuffer(frameContext->commandBuffer, &beginInfo) != VK_SUCCESS) {
		printf("failed to begin recording command buffer!");
		return;
	}

	// Transition image layout to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapChainImages[frameContext->imageIndex], // You'll need to store swapChainImages
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vkCmdPipelineBarrier(
		frameContext->commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

}
void endFrame(frameContext *frameContext, VkSwapchainKHR swapChain, VkQueue presentQueue, VkQueue graphicsQueue) {

	if (vkEndCommandBuffer(frameContext->commandBuffer) != VK_SUCCESS) {
		printf("failed to record command buffer!");
		return;
	}

	VkSemaphore waitSemaphores[] = {frameContext->imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signalSemaphores[] = {frameContext->renderFinishedSemaphore};

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &frameContext->commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameContext->RenderFinishedFence) != VK_SUCCESS) {
		printf("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pImageIndices = &frameContext->imageIndex,
		.pSwapchains = &swapChain,
		.pResults = nullptr
	};

	vkQueuePresentKHR(presentQueue, &presentInfo);
}

