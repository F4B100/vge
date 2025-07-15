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

typedef struct VgePipelineGraphicsCreateInfo_t {
	char * vertShaderPath;
	char * fragShaderPath;
	VkExtent2D viewportExtent;
	VkDevice device;
	VkFormat colorFormat;
	VkRenderPass renderPass;
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
