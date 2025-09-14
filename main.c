
#include "src/game/vgeMain.h"
#include "src/graphics/vulkan/vulkanInit.h"
#include "src/graphics/vulkan/vulkanRender.h"
#include "src/graphics/vulkan/vulkanSwapChain.h"
#include "src/parsers/obj/objParser.h"
#include "src/utils/camera.h"

typedef struct GameInfo {
	pVgeWindow window;
	pVulkanContext graphics;
	vgePipelineGraphics *graphicsP;
	pVgeVector model;
	pVgeDescriptor descriptor;
	pVgeVulkanTexture texture;

	uint32_t frameContexCount;
	uint32_t currentFrame;
	frameContext *frameContext;

	uint32_t lastMouseX, lastMouseY;
	float pitch, yaw;

	double timeLastFrame;
	double deltaTime;
	uint32_t frameCount;
	double timeElapsed;
} gameInfo, *pGameInfo;

void OnResize(pVgeWindow window, void *data, int32_t width, int32_t height) {
	pGameInfo info = (pGameInfo)data;
    if (width == 0 || height == 0) {
        return;
    }
    cleanupSwapChain(info->graphics->frameBuffers, info->graphics->swapChainImageViews, info->graphics->swapChainImages,
        info->graphics->swapchain, info->graphics->swapChainImageCount, info->graphics->device);
    createFullSwapChain(info->graphics->renderPass,
        info->graphics->physicalDevice,
        info->graphics->device,
        info->graphics->surface,
        info->window,
        &info->graphics->swapChainExtent,
        &info->graphics->swapchain,
        &info->graphics->swapChainImageCount,
        &info->graphics->swapChainImages,
        VK_FORMAT_R8G8B8A8_SRGB,
        &info->graphics->swapChainImageViews,
        &info->graphics->frameBuffers
        );
}

void OnMouseMove(pVgeWindow window, void *data, int32_t x, int32_t y) {
}

void GameStart(void *data) {
	pGameInfo info = (pGameInfo)data;

	vgeInit();

	info->window = vgeWindowInit(
		1280, 720,
		"Vge Window"
	);

	vgeSetCallbackData(info->window, data);
	vgeSetWindowSizeCallback(info->window, OnResize);
	vgeSetMouseMoveCallback(info->window, OnMouseMove);

	pVulkanContext context = initVulkan(info->window);

	info->graphics = context;

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

	info->graphicsP = pipeline;

	info->frameCount = 0;
	info->currentFrame = 0;

	info->pitch = 0.0f;info->yaw = 0.0f;

	info->frameContexCount = info->graphics->swapChainImageCount;

	info->frameContext = createFrameContext(info->frameContexCount, info->graphics->swapChainImageCount, info->graphics->device, info->graphics->commandPool);

	info->model = parseObjFile(context, pipeline, "model/squirCuble.obj");

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
		.pipeline = info->graphicsP
	};

	info->descriptor = createVgeDescriptorSet(context, &descriptorInfo);
}
void GameLoop(void *data) {
	pGameInfo info = (pGameInfo)data;

	info->timeLastFrame = vgeGetTimeSinceStart();
	const double currentFrame = vgeGetTimeSinceStart();
	info->deltaTime = currentFrame - info->timeLastFrame;
	info->timeLastFrame = currentFrame;

	startFrame(&info->frameContext[info->currentFrame], info->graphics->device, info->graphics->swapchain, info->graphics->swapChainImages);

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
	mat4 *model = mapUniformBindingData(info->graphics, info->descriptor, 0, 0, sizeof(mat4));
	memcpy(model, mat, sizeof(mat));
	unmapUniformBindingData(info->graphics, info->descriptor, 0);


	pVgeCamera camera = vgeCameraCreate(0.0f, 0.0f, 90.0f, 1280.0f / 720.0f, 0.1f, 3000.0f);
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

	for (uint64_t i = 0; i < vgeVectorGetSize(info->model); i++) {
		pObjModel model1 = vgeVectorGetElement(info->model, i);
		drawModel(info->graphics, &info->frameContext[info->currentFrame], info->graphicsP, model1->model, info->descriptor);
	}

	renderPassEnd(&info->frameContext[info->currentFrame]);

	if (info->timeElapsed > 1.0f) {
		printf("fps: %d\n", info->frameCount);
		info->timeElapsed -= 1.0f;
		info->frameCount = 0;
	}
	info->timeElapsed += info->deltaTime;
	info->frameCount++;

	endFrame(&info->frameContext[info->currentFrame], info->graphics->swapchain, info->graphics->queues[1], info->graphics->queues[0]);

	info->currentFrame = (info->currentFrame + 1) % info->graphics->swapChainImageCount;

	vgeHandleEvents();
}

uint32_t exitCondition(void *info) {
	return !vgeIsWindowClosed(((pGameInfo)info)->window);
}

void GameEnd(void *data) {
	pGameInfo info = (pGameInfo)data;
	destroyFrameContext(info->frameContext, info->graphics->device, info->frameContexCount, info->graphics->swapChainImageCount);

	for (uint64_t i = 0; i < vgeVectorGetSize(info->model); i++) {
		pObjModel model = vgeVectorGetElement(info->model, i);
		destroyVgeModel(info->graphics, model->model);
	}

	destroyGraphicsPipeline(info->graphics->device, info->graphicsP);
	destroyVulkan(info->graphics);
}

int main(void) {
	startEngine(sizeof(gameInfo));
}

