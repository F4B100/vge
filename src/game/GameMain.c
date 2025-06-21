//
// Created by fabio on 22-05-2025.
//

#include "GameMain.h"
#include <stdio.h>

void GameInit() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    gameInfo info;

    info.window = vgeWindowInit(
        600, 800,
        "hello world!",
        nullptr,
        nullptr
    );

    glfwShowWindow(info.window->window);

    glfwMakeContextCurrent(info.window->window);

    vulkanContext *context = initVulkan(info.window);

    info.graphics = context;

    VgePipelineGraphicsCreateInfo pipelineInfo = {
        .device = context->device,
        .fragShaderPath = "shaders_bin/rectangle.frag.spv",
        .vertShaderPath = "shaders_bin/rectangle.vert.spv",
        .colorFormat = VK_FORMAT_R8G8B8_SRGB,
        .viewportExtent = context->swapChainExtent
    };

    VgePipelineGraphics *pipeline = createGraphicsPipeline(&pipelineInfo);

    info.graphicsP = pipeline;

    GameStart(&info);

    destroyVulkan(context);
}
void GameStart(gameInfo *info) {
    double lastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(info->window->window)) {
        const double currentFrame = glfwGetTime();
        info->deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        GameLoop(info);
        glfwSwapBuffers(info->window->window);
        glfwPollEvents();
    }
}
void GameLoop(gameInfo *info) {
    if (info->timeElapsed > 1.0f) {
        printf("fps: %d\n", info->frameCount);
        info->timeElapsed -= 1.0f;
        info->frameCount = 0;
    }
    info->timeElapsed += info->deltaTime;
    info->frameCount++;
}