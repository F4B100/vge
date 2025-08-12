//
// Created by fabio on 22-05-2025.
//

#include "GameMain.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../../cmake-build-debug/_deps/cglm-src/include/cglm/call/cam.h"

gameInfo info;

void OnResize(pVgeWindow window, int32_t width, int32_t height) {
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

void OnMouseMove(pVgeWindow window, int32_t x, int32_t y) {
	uint32_t width, height;
	vgeGetContentSize(window, &width, &height);
	float yaw = ((float)x / width) * 3.14159, pitch = ((float)y / height) * 3.14159;

	printf("yaw: %f pitch: %f\n", yaw, pitch);

	mat4 rotation = GLM_MAT4_IDENTITY_INIT;

	vec3 pos = {1.0f, 1.0f, 0.0f};

	glm_translate(rotation, pos);

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			printf("%f|", rotation[j][i]);
		}
		printf("\n");
	}

	printf("=========================================\n");

	memcpy(info.model->indexInfo->data, rotation, sizeof(mat4));

	updateUniformsFromModel(info.graphics, info.model);

	vec4 rotation2 = {};

	vec4 pos2 = {0.0f, 0.0f, 0.0f, 1.0f};

	glm_mat4_mulv(rotation, pos2, rotation2);

	for (int i = 0; i < 4; ++i) {
		printf("%f|", rotation2[i]);
	}
	printf("\n");
}

void GameInit() {
    vgeInit();

	info.window = vgeWindowInit(
		1280, 720,
		"Vge Window"
	);

    vgeSetWindowSizeCallback(info.window, OnResize);
	vgeSetMouseMoveCallback(info.window, OnMouseMove);

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

    vgePipelineGraphics *pipeline = createGraphicsPipeline(&pipelineInfo);

    info.graphicsP = pipeline;

    info.frameCount = 0;
    info.currentFrame = 0;

	info.pitch = 0.0f;info.yaw = 0.0f;

    info.frameContexCount = info.graphics->swapChainImageCount;

    info.frameContext = createFrameContext(info.frameContexCount, info.graphics->swapChainImageCount, info.graphics->device, info.graphics->commandPool);

	info.model = createDefaultModel(context, info.graphicsP, VGE_MODEL_TRIANGLE);

    GameStart(&info);

    destroyFrameContext(info.frameContext, info.graphics->device, info.frameContexCount, info.graphics->swapChainImageCount);

	destroyVgeModel(info.graphics, info.model);

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

	drawModel(info->graphics, &info->frameContext[info->currentFrame], info->graphicsP, info->model);

    renderPassEnd(&info->frameContext[info->currentFrame]);

	if (info->timeElapsed > 1.0f) {
		printf("fps: %d\n", info->frameCount);
		info->timeElapsed -= 1.0f;
		info->frameCount = 0;
	}
	info->timeElapsed += info->deltaTime;
	info->frameCount++;
}