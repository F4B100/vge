//
// Created by fabio on 22-05-2025.
//

#include "GameMain.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../parsers/obj/objParser.h"

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

	vgeVertexDescription vertexInputDescriptor = {
		.binding = 0,
		.stride = 8 * sizeof(float)
	};

	vgeVertexInputDescriptor vertexInputInfo[3] = {
		{
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = 0,
			.location = 0
		},
		{
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = 3 * sizeof(float),
			.location = 1
		},
		{
			.binding = 0,
			.format = VK_FORMAT_R32G32_SFLOAT,
			.offset = 6 * sizeof(float),
			.location = 2
		}
	};

    VgePipelineGraphicsCreateInfo pipelineInfo = {
        .device = context->device,
        .fragShaderPath = "shaders_bin/objDefault.frag.spv",
        .vertShaderPath = "shaders_bin/objDefault.vert.spv",
        .colorFormat = VK_FORMAT_R8G8B8A8_SRGB,
        .viewportExtent = context->swapChainExtent,
        .renderPass = context->renderPass,
    	.numVertexDescriptions = 1,
    	.vertexDescriptionInfo = &vertexInputDescriptor,
    	.numVertexInputDescriptions = 3,
    	.vertexInputInfo = vertexInputInfo

    };

    vgePipelineGraphics *pipeline = createGraphicsPipeline(&pipelineInfo);

    info.graphicsP = pipeline;

    info.frameCount = 0;
    info.currentFrame = 0;

	info.pitch = 0.0f;info.yaw = 0.0f;

    info.frameContexCount = info.graphics->swapChainImageCount;

    info.frameContext = createFrameContext(info.frameContexCount, info.graphics->swapChainImageCount, info.graphics->device, info.graphics->commandPool);

	info.model = parseObjFile(context, pipeline, "model/monkey.obj");
	//info.model = parseObjFile(context, pipeline, "model/monkey.obj");

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
	mat4 mat = GLM_MAT4_IDENTITY_INIT;

	mat4 *model = mapUniformBindingData(info->graphics, info->model, 0, 0, sizeof(mat4));

	vec3 w = {0.0f, 0.0f, 0.0f};
	glm_translate(mat, w);

	vec3 up = {1.0f, 1.0f, 0.0f};
	glm_rotate(mat, vgeGetTimeSinceStart(), up);

	memcpy(model, mat, sizeof(mat));
	unmapUniformBindingData(info->graphics, info->model, 0);


	pVgeCamera camera = vgeCameraCreate(0.0f, 0.0f, 90.0f, 1280.0f / 720.0f, 0.1f, 300.0f);

	vec3 newPos = {0.0f, 0.0f, 3.0f};

	cameraSetPositon(camera, newPos);

	cameraSetRotation(camera, glm_rad(90.0f), glm_rad(180.0f));

	mat4 *view = mapUniformBindingData(info->graphics, info->model, 0, sizeof(mat4), sizeof(mat4));

	memcpy(view, getViewMatrix(camera), sizeof(mat));
	unmapUniformBindingData(info->graphics, info->model, 0);

	mat4 *perspective = mapUniformBindingData(info->graphics, info->model, 0, 2 * sizeof(mat4), sizeof(mat4));

	memcpy(perspective, getPerspectiveMatrix(camera), sizeof(mat));
	unmapUniformBindingData(info->graphics, info->model, 0);

	updateUniformBinding(info->graphics, info->model, 0);
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