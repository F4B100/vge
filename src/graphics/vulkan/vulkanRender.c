//
// Created by fabio on 20-06-2025.
//

#include "vulkanRender.h"

frameContext *createFrameContext(uint32_t numberFrame, uint32_t numberImages, VkDevice device, VkCommandPool commandPool) {
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

	swapChainSem *sem = calloc(numberImages, sizeof(swapChainSem));

	for (int i = 0; i < numberImages; ++i) {
		vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &sem[i].renderFinishedSemaphore);
	}

	frameContext *frameContext = calloc(numberFrame, sizeof(struct FrameContext));

	for (int i = 0; i < numberFrame; i++) {
		vkCreateFence(device, &fenceCreateInfo, nullptr, &frameContext[i].RenderFinishedFence);
		vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frameContext[i].imageAvailableSemaphore);
		vkAllocateCommandBuffers(device, &allocInfo, &frameContext[i].commandBuffer);
		frameContext[i].swapSemaphores = sem;
	}

	return frameContext;
}
void destroyFrameContext(frameContext *frameContext, VkDevice device, uint32_t numberFrames, uint32_t numberImages) {
	vkDeviceWaitIdle(device);
	for (int i = 0; i < numberImages; ++i) {
		vkDestroySemaphore(device, frameContext[0].swapSemaphores[i].renderFinishedSemaphore, nullptr);
	}
	for (int i = 0; i < numberFrames; i++) {
		vkWaitForFences(device, 1, &frameContext[i].RenderFinishedFence, VK_TRUE, UINT64_MAX);
		vkDestroyFence(device, frameContext[i].RenderFinishedFence, nullptr);
		vkDestroySemaphore(device, frameContext[i].imageAvailableSemaphore, nullptr);

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
	vkWaitForFences(device, 1, &frameContext->RenderFinishedFence, VK_TRUE, UINT64_MAX);

	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, frameContext->imageAvailableSemaphore, VK_NULL_HANDLE, &frameContext->imageIndex);

	vkResetFences(device, 1, &frameContext->RenderFinishedFence);

	vkResetCommandBuffer(frameContext->commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = NULL
	};

	if (vkBeginCommandBuffer(frameContext->commandBuffer, &beginInfo) != VK_SUCCESS) {
		printf("failed to begin recording command buffer!");
		return;
	}

	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapChainImages[frameContext->imageIndex],
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

void renderPassStart(frameContext *frameContext, VkRenderPass renderPass, VkExtent2D extent, VkFramebuffer *frameBuffers, VkClearValue *clearValues) {
	VkRenderPassBeginInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = renderPass,
		.framebuffer = frameBuffers[frameContext->imageIndex],
		.renderArea.offset = {0, 0},
		.renderArea.extent = extent,
		.clearValueCount = 1,
		.pClearValues = clearValues
	};

	vkCmdBeginRenderPass(frameContext->commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void renderPassEnd(frameContext *frameContext) {
	vkCmdEndRenderPass(frameContext->commandBuffer);
}

void drawModel(pVulkanContext context, frameContext *frameContext, pVgePipelineGraphics graphicsPipeline, pVgeModel model, pVgeDescriptor descriptor) {
	vkCmdBindPipeline(frameContext->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline);

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float) context->swapChainExtent.width,
		.height = (float) context->swapChainExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	vkCmdSetViewport(frameContext->commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = context->swapChainExtent
	};

	vkCmdSetScissor(frameContext->commandBuffer, 0, 1, &scissor);

	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(frameContext->commandBuffer, 0, 1, &model->vertexBuffer->buffer, offsets);
	vkCmdBindIndexBuffer(frameContext->commandBuffer, model->indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(frameContext->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout, 0, 1, &descriptor->descriptorSet, 0, nullptr);
	vkCmdDrawIndexed(frameContext->commandBuffer, model->indexInfo.numIndexes, 1, 0, 0, 0);
}

void endFrame(frameContext *frameContext, VkSwapchainKHR swapChain, VkQueue presentQueue, VkQueue graphicsQueue) {

	if (vkEndCommandBuffer(frameContext->commandBuffer) != VK_SUCCESS) {
		printf("failed to record command buffer!");
		return;
	}

	VkSemaphore waitSemaphores[] = {frameContext->imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signalSemaphores[] = {frameContext->swapSemaphores[frameContext->imageIndex].renderFinishedSemaphore};

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
		printf("failed to submit draw command buffer!\n");
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

