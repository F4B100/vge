//
// Created by fabio on 22-05-2025.
//

#include <stdio.h>
#include "GameMain.h"

gameInfo info;

void OnResize(GLFWwindow* window, int width, int height) {
    printf("resized x:%d y:%d\n", width, height);
    if (width == 0 || height == 0) {
        return;
    }
    vkDeviceWaitIdle(info.graphics->device);
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
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    info.window = vgeWindowInit(
        600, 800,
        "hello world!",
        nullptr,
        nullptr
    );

    glfwShowWindow(info.window->window);

    glfwMakeContextCurrent(info.window->window);

    glfwSetWindowSizeCallback(info.window->window, OnResize);

    vulkanContext *context = initVulkan(info.window);

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

    info.frameContext = createFrameContext(info.frameContexCount, info.graphics->device, info.graphics->commandPool);

    GameStart(&info);

    destroyFrameContext(info.frameContext, info.graphics->device, info.frameContexCount);

    destroyGraphicsPipeline(context->device, pipeline);
    destroyVulkan(context);
}
void GameStart(gameInfo *info) {
    double lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(info->window->window)) {
        const double currentFrame = glfwGetTime();
        info->deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        startFrame(&info->frameContext[info->currentFrame], info->graphics->device, info->graphics->swapchain, info->graphics->swapChainImages);

        GameLoop(info);

        endFrame(&info->frameContext[info->currentFrame], info->graphics->swapchain, info->graphics->queues[1], info->graphics->queues[0]);

        info->currentFrame = (info->currentFrame + 1) % info->graphics->swapChainImageCount;

        glfwSwapBuffers(info->window->window);
        glfwPollEvents();
    }
}
void GameLoop(gameInfo *info) {

    VkClearValue clearColor = {
        .color = {
            (float) cos( glfwGetTime()),
            ((float) cos(glfwGetTime() * 2.0f) + 1.0f) / 2.0f,
            ((float) sin(glfwGetTime() * 2.0f) + 1.0f) / 2.0f,
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