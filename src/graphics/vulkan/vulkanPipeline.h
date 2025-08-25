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
} VgePipelineGraphicsCreateInfo;

typedef struct VgePipelineGraphics_t {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorSetLayout descriptorSetLayout;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
} vgePipelineGraphics, *pVgePipelineGraphics;

vgePipelineGraphics *createGraphicsPipeline(VgePipelineGraphicsCreateInfo *info);
void createDescriptorPool(VkDevice device, VkDescriptorPool *toCreate);
void createRenderPass(VkDevice device, VkFormat swapChainImageFormat, VkRenderPass *toCreate);
void destroyGraphicsPipeline(VkDevice device, vgePipelineGraphics *pipeline);

#endif //VULKANPIPELINE_H
