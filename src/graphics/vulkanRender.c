//
// Created by fabio on 20-06-2025.
//

#include "vulkanRender.h"

void drawFrame() {
    vkWaitForFences(context->device, 1, &context->inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(context->device, context->surface.swapChain, UINT64_MAX, context->imageAvailableSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain(vulkan, window);
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        printf("failed to acquire swap chain image!\n");
    }
    vkResetFences(context->device, 1, &context->inFlightFence[currentFrame]);

    vkResetCommandBuffer(context->commandBuffer[currentFrame], 0);
    VkBuffer buffers[2] = {
        rect->vertexBuffer,
        rect->indexBuffer
    };

    recordCommandBuffer(context, buffers, context->commandBuffer[currentFrame], imageIndex);

    VkSemaphore waitSemaphores[] = {context->imageAvailableSemaphore[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {context->renderFinishedSemaphore[currentFrame]};

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &context->commandBuffer[currentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    if (vkQueueSubmit(context->graphicsQueue, 1, &submitInfo, context->inFlightFence[currentFrame]) != VK_SUCCESS) {
        printf("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pImageIndices = &imageIndex,
        .pSwapchains = &context->surface.swapChain,
        .pResults = nullptr
    };

    vkQueuePresentKHR(context->presentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
