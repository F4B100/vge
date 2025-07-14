//
// Created by fabio on 22-05-2025.
//

#include <stdio.h>
#include <math.h>
#include "GameMain.h"

gameInfo info;

void OnResize(pVgeWindow window, uint32_t width, uint32_t height) {
    if (width == 0 || height == 0) {
        return;
    }
    cleanupSwapChain(info.graphics->frameBuffers, info.graphics->swapChainImageViews, info.graphics->swapChainImages,
        info.graphics->swapchain, info.graphics->swapChainImageCount, info.graphics->device);
    createFullSwapChain(info.graphics->renderPass,
        info.graphics->physicalDevice,
        info.graphics->device,
        info.graphics->surface,
        info.window,
        &info.graphics->swapChainExtent,
        &info.graphics->swapchain,
        &info.graphics->swapChainImageCount,
        &info.graphics->swapChainImages,
        VK_FORMAT_R8G8B8A8_SRGB,
        &info.graphics->swapChainImageViews,
        &info.graphics->frameBuffers
        );
}

void GameInit() {
    vgeInit();

	info.window = vgeWindowInit(
		1280, 720,
		"Vge Window"
	);

    vgeSetWindowSizeCallback(info.window, OnResize);

    pVulkanContext context = initVulkan(info.window);

    info.graphics = context;

    VgePipelineGraphicsCreateInfo pipelineInfo = {
        .device = context->device,
        .fragShaderPath = "shaders_bin/rectangle.frag.spv",
        .vertShaderPath = "shaders_bin/rectangle.vert.spv",
        .colorFormat = VK_FORMAT_R8G8B8A8_SRGB,
        .viewportExtent = context->swapChainExtent,
        .renderPass = context->renderPass
    };

    VgePipelineGraphics *pipeline = createGraphicsPipeline(&pipelineInfo);

    info.graphicsP = pipeline;

    info.frameCount = 0;
    info.currentFrame = 0;

    info.frameContexCount = info.graphics->swapChainImageCount;

    info.frameContext = createFrameContext(info.frameContexCount, info.graphics->swapChainImageCount, info.graphics->device, info.graphics->commandPool);

    GameStart(&info);

    destroyFrameContext(info.frameContext, info.graphics->device, info.frameContexCount, info.graphics->swapChainImageCount);

    destroyGraphicsPipeline(context->device, pipeline);
    destroyVulkan(context);
}
void GameStart(gameInfo *info) {
    double lastFrame = vgeGetTimeSinceStart();
    while (!vgeIsWindowClosed(info->window)) {
        const double currentFrame = vgeGetTimeSinceStart();
        info->deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        startFrame(&info->frameContext[info->currentFrame], info->graphics->device, info->graphics->swapchain, info->graphics->swapChainImages);

        GameLoop(info);

        endFrame(&info->frameContext[info->currentFrame], info->graphics->swapchain, info->graphics->queues[1], info->graphics->queues[0]);

        info->currentFrame = (info->currentFrame + 1) % info->graphics->swapChainImageCount;

    	vgeHandleEvents();
    }
}
void GameLoop(gameInfo *info) {

    VkClearValue clearColor = {
        .color = {
            (float) cos(vgeGetTimeSinceStart()),
            ((float) cos(vgeGetTimeSinceStart() * 2.0f) + 1.0f) / 2.0f,
            ((float) sin(vgeGetTimeSinceStart() * 2.0f) + 1.0f) / 2.0f,
            1.0f
        }
    };

    renderPassStart(&info->frameContext[info->currentFrame], info->graphics->renderPass, info->graphics->swapChainExtent, info->graphics->frameBuffers, &clearColor);

    if (info->timeElapsed > 1.0f) {
        printf("fps: %d\n", info->frameCount);
        info->timeElapsed -= 1.0f;
        info->frameCount = 0;
    }
    info->timeElapsed += info->deltaTime;
    info->frameCount++;

    renderPassEnd(&info->frameContext[info->currentFrame]);
}