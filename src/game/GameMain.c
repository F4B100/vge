//
// Created by fabio on 22-05-2025.
//

#include "GameMain.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../parsers/obj/objParser.h"

gameInfo infoGlobal;

void OnResize(pVgeWindow window, int32_t width, int32_t height) {
    if (width == 0 || height == 0) {
        return;
    }
    cleanupSwapChain(infoGlobal.graphics->frameBuffers, infoGlobal.graphics->swapChainImageViews, infoGlobal.graphics->swapChainImages,
        infoGlobal.graphics->swapchain, infoGlobal.graphics->swapChainImageCount, infoGlobal.graphics->device);
    createFullSwapChain(infoGlobal.graphics->renderPass,
        infoGlobal.graphics->physicalDevice,
        infoGlobal.graphics->device,
        infoGlobal.graphics->surface,
        infoGlobal.window,
        &infoGlobal.graphics->swapChainExtent,
        &infoGlobal.graphics->swapchain,
        &infoGlobal.graphics->swapChainImageCount,
        &infoGlobal.graphics->swapChainImages,
        VK_FORMAT_R8G8B8A8_SRGB,
        &infoGlobal.graphics->swapChainImageViews,
        &infoGlobal.graphics->frameBuffers
        );
}

void OnMouseMove(pVgeWindow window, int32_t x, int32_t y) {
}

void GameInit() {
	vgeInit();

	infoGlobal.window = vgeWindowInit(
		1280, 720,
		"Vge Window"
	);

	vgeSetWindowSizeCallback(infoGlobal.window, OnResize);
	vgeSetMouseMoveCallback(infoGlobal.window, OnMouseMove);

	pVulkanContext context = initVulkan(infoGlobal.window);

	infoGlobal.graphics = context;

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

	vgeDescriptorLayoutInfo descriptorLayoutInfo[2] = {
		{
			.binding = 0,
			.type = VGE_PIPELINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.count = 1,
			.stage = VGE_PIPELINE_DESCRIPTOR_STAGE_VERTEX
		},
		{
			.binding = 1,
			.type = VGE_PIPELINE_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.count = 1,
			.stage = VGE_PIPELINE_DESCRIPTOR_STAGE_FRAGMENT
		}
	};

	vgePipelineGraphicsCreateInfo pipelineInfo = {
		.device = context->device,
		.fragShaderPath = "shaders_bin/objDefault.frag.spv",
		.vertShaderPath = "shaders_bin/objDefault.vert.spv",
		.colorFormat = VK_FORMAT_R8G8B8A8_SRGB,
		.viewportExtent = context->swapChainExtent,
		.renderPass = context->renderPass,
		.numVertexDescriptions = 1,
		.vertexDescriptionInfo = &vertexInputDescriptor,
		.numVertexInputDescriptions = 3,
		.vertexInputInfo = vertexInputInfo,
		.numDescriptorLayoutInfo = 2,
		.descriptorLayoutInfo = descriptorLayoutInfo
	};

	vgePipelineGraphics *pipeline = createGraphicsPipeline(&pipelineInfo);

	infoGlobal.graphicsP = pipeline;

	infoGlobal.frameCount = 0;
	infoGlobal.currentFrame = 0;

	infoGlobal.pitch = 0.0f;infoGlobal.yaw = 0.0f;

	infoGlobal.frameContexCount = infoGlobal.graphics->swapChainImageCount;

	infoGlobal.frameContext = createFrameContext(infoGlobal.frameContexCount, infoGlobal.graphics->swapChainImageCount, infoGlobal.graphics->device, infoGlobal.graphics->commandPool);

	infoGlobal.model = parseObjFile(context, pipeline, "model/monkey.obj");
	infoGlobal.model2 = parseObjFile(context, pipeline, "model/Untitled.obj");

	vgeBindingInfo infoBinding [2] = {
		{
			.binding = 0,
			.bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER,
			.uniformInfo = {
				.option = VGE_UNIFORM_CREATE_BUFFER,
				.sizeUniform = sizeof(mat4) * 3
			}
		},
		{
			.binding = 1,
			.bindingType = VGE_BINDING_TYPE_TEXTURE,
			.textureInfo = {
				.option = VGE_TEXTURE_NOT_INITIALIZED,
				.TexturePath = "textures/img.png"
			}
		}
	};

	vgeDescriptorInfo descriptorInfo = {
		.numBindings = 2,
		.bindings = infoBinding,
		.pipeline = infoGlobal.graphicsP
	};

	infoGlobal.descriptor = createVgeDescriptorSet(context, &descriptorInfo);

    GameStart(&infoGlobal);

    destroyFrameContext(infoGlobal.frameContext, infoGlobal.graphics->device, infoGlobal.frameContexCount, infoGlobal.graphics->swapChainImageCount);

	destroyVgeModel(infoGlobal.graphics, infoGlobal.model);

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

	vec3 rotationAxis = {0.0f, 1.0f, 1.0f};
	vec3 scaleVec = {1.0f, 2.0f, 1.0f};

	mat4 *model = mapUniformBindingData(info->graphics, info->descriptor, 0, 0, sizeof(mat4));

	glm_scale(mat, scaleVec);

	glm_rotate(mat, vgeGetTimeSinceStart(), rotationAxis);

	memcpy(model, mat, sizeof(mat));
	unmapUniformBindingData(info->graphics, info->descriptor, 0);


	pVgeCamera camera = vgeCameraCreate(0.0f, 0.0f, 90.0f, 1280.0f / 720.0f, 0.1f, 300.0f);

	vec3 newPos = {0.0f, 0.0f, 3.0f};

	cameraSetPositon(camera, newPos);

	cameraSetRotation(camera, glm_rad(90.0f), glm_rad(180.0f));

	mat4 *view = mapUniformBindingData(info->graphics, info->descriptor, 0, sizeof(mat4), sizeof(mat4));

	memcpy(view, getViewMatrix(camera), sizeof(mat));
	unmapUniformBindingData(info->graphics, info->descriptor, 0);

	mat4 *perspective = mapUniformBindingData(info->graphics, info->descriptor, 0, 2 * sizeof(mat4), sizeof(mat4));

	memcpy(perspective, getPerspectiveMatrix(camera), sizeof(mat));
	unmapUniformBindingData(info->graphics, info->descriptor, 0);

	updateUniformBinding(info->graphics, info->descriptor, 0);
	drawModel(info->graphics, &info->frameContext[info->currentFrame], info->graphicsP, info->model, info->descriptor);

	renderPassEnd(&info->frameContext[info->currentFrame]);

	if (info->timeElapsed > 1.0f) {
		printf("fps: %d\n", info->frameCount);
		info->timeElapsed -= 1.0f;
		info->frameCount = 0;
	}
	info->timeElapsed += info->deltaTime;
	info->frameCount++;
}