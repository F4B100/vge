//
// Created by fabio on 6/16/2025.
//

#ifndef VULKANPIPELINE_H
#define VULKANPIPELINE_H

#include "vulkanDefs.h"


typedef struct SpirVCode {
	uint64_t size;
	uint32_t *code;
} spirVCode;

typedef struct VgeVertexInputDescriptor{
	uint32_t binding;
	uint32_t location;
	VkFormat format;
	uint32_t offset;
} vgeVertexInputDescriptor, *pVgeVertexInputDescriptor;

typedef struct VgeVertexDescription{
	uint32_t binding;
	uint32_t stride;
} vgeVertexDescription, *pVgeVertexDescription;

#define VGE_PIPELINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER			0x0000
#define VGE_PIPELINE_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER 0x0001

#define VGE_PIPELINE_DESCRIPTOR_STAGE_VERTEX				0x0000
#define VGE_PIPELINE_DESCRIPTOR_STAGE_FRAGMENT				0x0001

typedef struct VgeDescriptorLayoutInfo {
	uint32_t binding;
	uint32_t type;
	uint32_t stage;
	uint32_t count;
} vgeDescriptorLayoutInfo, *pVgeDescriptorLayoutInfo;

typedef struct VgePipelineGraphicsCreateInfo_t {
	char * vertShaderPath;
	char * fragShaderPath;
	VkExtent2D viewportExtent;
	VkDevice device;
	VkFormat colorFormat;
	VkRenderPass renderPass;
	uint64_t numVertexDescriptions;
	pVgeVertexDescription vertexDescriptionInfo;
	uint64_t numVertexInputDescriptions;
	pVgeVertexInputDescriptor vertexInputInfo;
	uint64_t numDescriptorLayoutInfo;
	pVgeDescriptorLayoutInfo descriptorLayoutInfo;
} vgePipelineGraphicsCreateInfo, *pVgePipelineGraphicsCreateInfo;

typedef struct VgePipelineGraphics_t {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
} vgePipelineGraphics, *pVgePipelineGraphics;

vgePipelineGraphics *createGraphicsPipeline(pVgePipelineGraphicsCreateInfo info);
void createDescriptorPool(VkDevice device, VkDescriptorPool *toCreate);
void createRenderPass(VkDevice device, VkFormat swapChainImageFormat, VkRenderPass *toCreate);
void destroyGraphicsPipeline(VkDevice device, vgePipelineGraphics *pipeline);

#endif //VULKANPIPELINE_H
