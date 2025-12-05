
#include <string.h>
#include <unistd.h>

#include "src/game/vgeMain.h"
#include "src/graphics/vulkan/vulkanInit.h"
#include "src/graphics/vulkan/vulkanRender.h"
#include "src/graphics/vulkan/vulkanSwapChain.h"
#include "src/parsers/obj/objParser.h"
pVgeWindow parseWindowConfFile(char * filename);
#include "src/utils/camera.h"

typedef struct GameInfo {
	pVgeWindow window;
	pVulkanContext context;
	pVulkanSwapchain swapChain;
	vgePipelineGraphics *graphicsP;
	pVgeVector model;
	pVgeDescriptor descriptor;
	pVgeDescriptor descriptor2;

	pVgeCamera camera;

	uint32_t frameContexCount;
	uint32_t currentFrame;
	frameContext *frameContext;

	float pitch, yaw;

	double timeLastFrame;
	double deltaTime;
	uint32_t frameCount;
	double timeElapsed;

	vgeKeyboard kb;

	uint32_t firstMouse;
} gameInfo, *pGameInfo;

void OnResize(pVgeWindow window, void *data, int32_t width, int32_t height) {
	pGameInfo info = data;
    if (width == 0 || height == 0) {
        return;
    }
	resizeSwapchain(info->context, info->swapChain);
	int32_t x, y;
	vgeGetContentSize(info->window, &x, &y);
	cameraSetAspectRatio(info->camera, (float)x / (float)y);
}

void OnMouseMove(pVgeWindow window, void *data, int32_t x, int32_t y) {
	pGameInfo info = data;
	if (x > 0 || y > 0) {
		//printf("%.6f|%d|%.6f|%d\n", (float)x * 0.0001f, x, (float)y * 0.0001f, y);
	}
	cameraRotate(info->camera , (float)x * 0.0001f, (float)y * 0.0001f);
	if (x > 0 || y > 0) {
		float yaw, pitch;
		cameraGetRotation(info->camera, &yaw, &pitch);
		printf("camera:%.6f|%.6f\n", yaw, pitch);
	}
}

void OnKeyDown(pVgeWindow window, void *data, int8_t key) {

}

void GameStart(void *data) {
	pGameInfo info = data;
	info->firstMouse = true;

	vgeInit();

	info->window = vgeWindowInit(1600, 1600, "mein window");
	if (info->window == NULL) {
		exit(1);
	}

	vgeSetWindowCaptureMouse(info->window, true);

	vgeSetCallbackData(info->window, data);
	vgeSetWindowSizeCallback(info->window, OnResize);
	vgeSetMouseMoveCallback(info->window, OnMouseMove);
	vgeSetKeyDownCallback(info->window, OnKeyDown);

	info->context = initVulkan("THA GAME");
	info->swapChain = createFullSwapChain(info->context, info->window);

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

	vgeDescriptorLayoutInfo descriptorLayoutInfo[6] = {
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
		},
		{
			.binding = 2,
			.type = VGE_PIPELINE_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.count = 1,
			.stage = VGE_PIPELINE_DESCRIPTOR_STAGE_FRAGMENT
		},
		{
			.binding = 3,
			.type = VGE_PIPELINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.count = 1,
			.stage = VGE_PIPELINE_DESCRIPTOR_STAGE_FRAGMENT
		},
		{
			.binding = 4,
			.type = VGE_PIPELINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.count = 1,
			.stage = VGE_PIPELINE_DESCRIPTOR_STAGE_FRAGMENT
		},
		{
			.binding = 5,
			.type = VGE_PIPELINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.count = 1,
			.stage = VGE_PIPELINE_DESCRIPTOR_STAGE_FRAGMENT
		}
	};

	vgePipelineGraphicsCreateInfo pipelineInfo = {
		.context = info->context,
		.swapchain = info->swapChain,
		.fragShaderPath = "shaders_bin/objDefault.frag.spv",
		.vertShaderPath = "shaders_bin/objDefault.vert.spv",
		.numVertexDescriptions = 1,
		.vertexDescriptionInfo = &vertexInputDescriptor,
		.numVertexInputDescriptions = 3,
		.vertexInputInfo = vertexInputInfo,
		.numDescriptorLayoutInfo = 6,
		.descriptorLayoutInfo = descriptorLayoutInfo
	};

	vgePipelineGraphics *pipeline = createGraphicsPipeline(&pipelineInfo);

	info->graphicsP = pipeline;

	info->frameCount = 0;
	info->currentFrame = 0;

	info->pitch = 0.0f;info->yaw = 0.0f;

	vgeBindingInfo infoBinding [6] = {
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
				.TexturePath = "textures/heavy.png"
			}
		},
		{
			.binding = 2,
			.bindingType = VGE_BINDING_TYPE_TEXTURE,
			.textureInfo = {
				.option = VGE_TEXTURE_NOT_INITIALIZED,
				.TexturePath = "textures/heavy.png"
			}
		},
		{
			.binding = 3,
			.bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER,
			.uniformInfo = {
				.option = VGE_UNIFORM_CREATE_BUFFER,
				.sizeUniform = sizeof(float)
			}
		},
		{
			.binding = 4,
			.bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER,
			.uniformInfo = {
				.option = VGE_UNIFORM_CREATE_BUFFER,
				.sizeUniform = sizeof(vec3) * 4
			}
		},
		{
			.binding = 5,
			.bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER,
			.uniformInfo = {
				.option = VGE_UNIFORM_CREATE_BUFFER,
				.sizeUniform = sizeof(vec3)
			}
		}
	};


	vgeDescriptorInfo descriptorInfo = {
		.numBindings = 6,
		.bindings = infoBinding,
		.pipeline = info->graphicsP
	};

	info->descriptor = createVgeDescriptorSet(info->context, &descriptorInfo);
	float *shiny = mapUniformBindingData(info->context, info->descriptor, 3, 0, sizeof(float));
	*shiny = 32.0f;
	unmapUniformBindingData(info->context, info->descriptor, 3);
	updateUniformBinding(info->context, info->descriptor, 3);

	float *lightInfo = mapUniformBindingData(info->context, info->descriptor, 4, sizeof(vec3), 3 * sizeof(vec3));
	vec3 LightInfo[1] = {
		{1.0f, 1.0f, 1.0f}
	};
	memcpy(lightInfo, LightInfo, sizeof(vec3) * 1);
	glm_vec3_print(lightInfo, stdout);
	unmapUniformBindingData(info->context, info->descriptor, 4);
	updateUniformBinding(info->context, info->descriptor, 4);

	info->timeLastFrame = vgeGetTimeSinceStart();

	info->model = parseObjFile(info->context, info->graphicsP, "model/Untitled.obj");

	int32_t x, y;
	vgeGetContentSize(info->window, &x, &y);
	info->camera = vgeCameraCreate(glm_rad(-90.0f), 0.0f, 60.0f, (float)x / (float)y, 0.1f, 3000.0f);
	vec3 pos = {0.0f, 0.0f, 3.0f};
	cameraMove(info->camera, pos);
}
void GameLoop(void *data) {
	pGameInfo info = data;

	const double currentFrame = vgeGetTimeSinceStart();
	info->deltaTime = currentFrame - info->timeLastFrame;
	info->timeLastFrame = currentFrame;

	pVgeKeyboard keyboard = vgeWindowGetKeyboard(info->window);

	if (keyboard->keys['W']) {
		vec3 front, dir;
		cameraGetFront(info->camera, front);
		glm_vec3_scale(front, (float)info->deltaTime * 10.0f, dir);
		dir[1] = 0.0f;
		cameraMove(info->camera, dir);
	}
	if (keyboard->keys['S']) {
		vec3 front, dir;
		cameraGetFront(info->camera, front);
		glm_vec3_scale(front, (float)info->deltaTime * -10.0f, dir);
		dir[1] = 0.0f;
		cameraMove(info->camera, dir);
	}
	if (keyboard->keys['A']) {
		vec3 front;
		cameraGetFront(info->camera, front);
		vec3 up = {0.0f, 1.0f, 0.0f};
		glm_cross(front, up, front);
		glm_vec3_normalize(front);
		glm_vec3_scale(front, (float)info->deltaTime * -10.0f, front);
		cameraMove(info->camera, front);
	}
	if (keyboard->keys['D']) {
		vec3 front;
		cameraGetFront(info->camera, front);
		vec3 up = {0.0f, 1.0f, 0.0f};
		glm_cross(front, up, front);
		glm_vec3_normalize(front);
		glm_vec3_scale(front, (float)info->deltaTime * 10.0f, front);
		cameraMove(info->camera, front);
	}
	if (keyboard->keys['Z']) {
		vec3 up = {0.0f, (float)info->deltaTime * -10.0f, 0.0f};
		cameraMove(info->camera, up);
	}
	if (keyboard->keys[' ']) {
		vec3 up = {0.0f, (float)info->deltaTime * 10.0f, 0.0f};
		cameraMove(info->camera, up);
	}
	if (keyboard->keys['C']) {
		vgeSetWindowCaptureMouse(info->window,true);
	}
	if (keyboard->keys['V']) {
			vgeSetWindowCaptureMouse(info->window,false);
	}

	startFrame(info->context, info->swapChain);

	mat4 mat = GLM_MAT4_IDENTITY_INIT;
	mat4 *model = mapUniformBindingData(info->context, info->descriptor, 0, 0, sizeof(mat4));
	memcpy(model, mat, sizeof(mat));
	unmapUniformBindingData(info->context, info->descriptor, 0);
	updateUniformBinding(info->context, info->descriptor, 0);

	float *light = mapUniformBindingData(info->context, info->descriptor, 4, 0, sizeof(vec3));
	vec3 lightPos = {0.0f, 3.0f, 3.0f};
	memcpy(light, lightPos, sizeof(vec3));
	unmapUniformBindingData(info->context, info->descriptor, 4);
	updateUniformBinding(info->context, info->descriptor, 4);

	float *cameraPos = mapUniformBindingData(info->context, info->descriptor, 5, 0, sizeof(vec3));
	memcpy(cameraPos, cameraGetPos(info->camera), sizeof(vec3));
	unmapUniformBindingData(info->context, info->descriptor, 5);
	updateUniformBinding(info->context, info->descriptor, 5);

	cameraBindMatrices(info->camera, info->context, info->descriptor, 0, sizeof(mat4));

	for (uint64_t i = 0; i < vgeVectorGetSize(info->model); i++) {
		pObjModel model1 = vgeVectorGetElement(info->model, i);
		drawModel(info->context, info->swapChain, info->graphicsP, model1->model, info->descriptor);
	}

	endFrame(info->context, info->swapChain);

	if (info->timeElapsed > 1.0f) {
		printf("fps: %d\n", info->frameCount);
		info->timeElapsed -= 1.0f;
		info->frameCount = 0;
	}
	info->timeElapsed += info->deltaTime;
	info->frameCount++;

	vgeHandleEvents();
}

uint32_t exitCondition(void *info) {
	uint32_t cond = !vgeIsWindowClosed(((pGameInfo)info)->window);
	return cond;
}

void GameEnd(void *data) {
	pGameInfo info = (pGameInfo)data;

	/*for (uint64_t i = 0; i < vgeVectorGetSize(info->model); i++) {
		pObjModel model = vgeVectorGetElement(info->model, i);
		destroyVgeModel(info->context, model->model);
	}

	destroyGraphicsPipeline(info->context->device, info->graphicsP);
	destroyVulkan(info->context);*/
}

int main(int argc, char *argv[]) {
	startEngine(sizeof(gameInfo));
}

