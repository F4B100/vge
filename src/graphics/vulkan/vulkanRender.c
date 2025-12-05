//
// Created by fabio on 20-06-2025.
//

#include "vulkanRender.h"

#include <stdio.h>
#include <stdlib.h>

frameContext *createFrameContext(pVulkanContext context, pVulkanSwapchain swapchain) {
	const VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = 0
	};
	const VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};
	const VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = context->commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	swapChainSem *sem = calloc(swapchain->swapChainImageCount, sizeof(swapChainSem));

	for (int i = 0; i < swapchain->swapChainImageCount; ++i) {
		vkCreateSemaphore(context->device, &semaphoreCreateInfo, nullptr, &sem[i].renderFinishedSemaphore);
	}

	frameContext *frameContext = calloc(swapchain->swapChainImageCount, sizeof(struct FrameContext));

	for (int i = 0; i < swapchain->swapChainImageCount; i++) {
		vkCreateFence(context->device, &fenceCreateInfo, nullptr, &frameContext[i].RenderFinishedFence);
		vkCreateSemaphore(context->device, &semaphoreCreateInfo, nullptr, &frameContext[i].imageAvailableSemaphore);
		vkAllocateCommandBuffers(context->device, &allocInfo, &frameContext[i].commandBuffer);
		frameContext[i].swapSemaphores = sem;
	}

	return frameContext;
}
void destroyFrameContext(pVulkanContext context, pVulkanSwapchain swapchain) {
	vkDeviceWaitIdle(context->device);
	for (int i = 0; i < swapchain->swapChainImageCount; ++i) {
		vkDestroySemaphore(context->device, swapchain->frameContext[0].swapSemaphores[i].renderFinishedSemaphore, nullptr);
	}
	for (int i = 0; i < swapchain->swapChainImageCount; i++) {
		vkWaitForFences(context->device, 1, &swapchain->frameContext[i].RenderFinishedFence, VK_TRUE, UINT64_MAX);
		vkDestroyFence(context->device, swapchain->frameContext[i].RenderFinishedFence, nullptr);
		vkDestroySemaphore(context->device, swapchain->frameContext[i].imageAvailableSemaphore, nullptr);

		VkCommandBufferBeginInfo beginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0,
			.pInheritanceInfo = nullptr
		};

		if (vkBeginCommandBuffer(swapchain->frameContext[i].commandBuffer, &beginInfo) != VK_SUCCESS) {
			printf("failed to begin recording command buffer!");
			return;
		}
		vkEndCommandBuffer(swapchain->frameContext[i].commandBuffer);
	}
}

void renderPassStart(pVulkanContext context, pVulkanSwapchain swapchain) {
	VkClearValue clearVals[2] = {
		{
			.color = swapchain->clearColor,
		},
		{
			.depthStencil ={
				.depth = 1.0f, .stencil = 0
			}
		}
	};
	VkRenderPassBeginInfo renderPassInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = swapchain->renderPass,
		.framebuffer = swapchain->frameBuffers[swapchain->frameContext[swapchain->currentFrame].imageIndex],
		.renderArea.offset = {0, 0},
		.renderArea.extent = swapchain->swapChainExtent,
		.clearValueCount = 2,
		.pClearValues = clearVals
	};

	vkCmdBeginRenderPass(swapchain->frameContext[swapchain->currentFrame].commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void renderPassEnd(pVulkanSwapchain swapchain) {
	vkCmdEndRenderPass(swapchain->frameContext[swapchain->currentFrame].commandBuffer);
}

void startFrame(pVulkanContext context, pVulkanSwapchain swapchain) {
	vkWaitForFences(context->device, 1, &swapchain->frameContext[swapchain->currentFrame].RenderFinishedFence, VK_TRUE, UINT64_MAX);

	vkAcquireNextImageKHR(context->device, swapchain->swapchain, UINT64_MAX, swapchain->frameContext[swapchain->currentFrame].imageAvailableSemaphore,
	                                        VK_NULL_HANDLE, &swapchain->frameContext[swapchain->currentFrame].imageIndex);

	vkResetFences(context->device, 1, &swapchain->frameContext[swapchain->currentFrame].RenderFinishedFence);

	vkResetCommandBuffer(swapchain->frameContext[swapchain->currentFrame].commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = 0,
		.pInheritanceInfo = nullptr
	};

	if (vkBeginCommandBuffer(swapchain->frameContext[swapchain->currentFrame].commandBuffer, &beginInfo) != VK_SUCCESS) {
		printf("failed to begin recording command buffer!");
		return;
	}

	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapchain->swapChainImages[swapchain->frameContext[swapchain->currentFrame].imageIndex],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	vkCmdPipelineBarrier(
		swapchain->frameContext[swapchain->currentFrame].commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	renderPassStart(context, swapchain);
}

void drawModel(pVulkanContext context, pVulkanSwapchain swapchain, pVgePipelineGraphics graphicsPipeline, pVgeModel model, pVgeDescriptor descriptor) {
	vkCmdBindPipeline(swapchain->frameContext[swapchain->currentFrame].commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline);

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = (float) swapchain->swapChainExtent.width,
		.height = (float) swapchain->swapChainExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	vkCmdSetViewport(swapchain->frameContext[swapchain->currentFrame].commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = swapchain->swapChainExtent
	};

	vkCmdSetScissor(swapchain->frameContext[swapchain->currentFrame].commandBuffer, 0, 1, &scissor);

	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(swapchain->frameContext[swapchain->currentFrame].commandBuffer, 0, 1, &model->vertexBuffer->buffer, offsets);
	vkCmdBindIndexBuffer(swapchain->frameContext[swapchain->currentFrame].commandBuffer, model->indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(swapchain->frameContext[swapchain->currentFrame].commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout, 0, 1, &descriptor->descriptorSet, 0, nullptr);
	vkCmdDrawIndexed(swapchain->frameContext[swapchain->currentFrame].commandBuffer, model->indexInfo.numIndexes, 1, 0, 0, 0);
}

void endFrame(pVulkanContext context, pVulkanSwapchain swapchain) {

	renderPassEnd(swapchain);

	if (vkEndCommandBuffer(swapchain->frameContext[swapchain->currentFrame].commandBuffer) != VK_SUCCESS) {
		printf("failed to record command buffer!");
		return;
	}

	VkSemaphore waitSemaphores[] = {swapchain->frameContext[swapchain->currentFrame].imageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore signalSemaphores[] = {swapchain->frameContext[swapchain->currentFrame].swapSemaphores[swapchain->frameContext[swapchain->currentFrame].imageIndex].renderFinishedSemaphore};

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = waitStages,
		.commandBufferCount = 1,
		.pCommandBuffers = &swapchain->frameContext[swapchain->currentFrame].commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphores
	};

	if (vkQueueSubmit(context->queues[0], 1, &submitInfo, swapchain->frameContext[swapchain->currentFrame].RenderFinishedFence) != VK_SUCCESS) {
		printf("failed to submit draw command buffer!\n");
	}

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphores,
		.swapchainCount = 1,
		.pImageIndices = &swapchain->frameContext[swapchain->currentFrame].imageIndex,
		.pSwapchains = &swapchain->swapchain,
		.pResults = nullptr
	};

	vkQueuePresentKHR(context->queues[0], &presentInfo);
	swapchain->currentFrame++;
	swapchain->currentFrame %= swapchain->swapChainImageCount;
}

