//
// Created by fabio on 9/7/2025.
//

#ifndef VGE_MODELDESCRIPTORS_H
#define VGE_MODELDESCRIPTORS_H

#include <stdint.h>
#include "../vulkan/vulkanPipeline.h"
#include "../vulkan/vulkanBuffer.h"
#include "../vulkan/vulkanTexture.h"

#define VGE_UNIFORM_CREATE_BUFFER			0x00
#define VGE_UNIFORM_USE_OUTSIDE_BUFFER		0x01

typedef struct VgeUniformBindingInfo {
	uint8_t option;
	uint64_t sizeUniform;
	uint64_t offsetUniform;
	pVulkanBuffer buffer;
} vgeUniformBindingInfo, *pVgeUniformBindingInfo;

#define VGE_TEXTURE_NOT_INITIALIZED			0x00
#define VGE_TEXTURE_INITIALIZED				0x01

typedef struct VgeTextureBindingInfo {
	uint8_t option;
	pVgeVulkanTexture texture;
	const char * TexturePath;
} vgeTextureBindingInfo, *pVgeTextureBindingInfo;

#define VGE_BINDING_TYPE_UNIFORM_BUFFER		0x00
#define VGE_BINDING_TYPE_TEXTURE			0x01

typedef struct VgeBindingInfo {
	uint32_t binding;
	uint8_t bindingType;
	vgeTextureBindingInfo textureInfo;
	vgeUniformBindingInfo uniformInfo;
} vgeBindingInfo, *pVgeBindingInfo;

typedef struct VgeDescriptorInfo {
	uint64_t numBindings;
	pVgeBindingInfo bindings;
	pVgePipelineGraphics pipeline;
} vgeDescriptorInfo, *pVgeDescriptorInfo;

typedef struct VgeUniformBinding {
	uint64_t size;
	uint64_t offset;
	pVulkanBuffer buffer;
} vgeUniformBinding, *pVgeUniformBinding;

typedef struct VgeTextureBinding {
	uint8_t option;
	pVgeVulkanTexture texture;
} vgeTextureBinding, *pVgeTextureBinding;

typedef struct VgeBinding {
	uint32_t binding;
	uint8_t bindingType;
	vgeTextureBinding texture;
	vgeUniformBinding uniform;
} vgeBinding, *pVgeBinding;

typedef struct VgeDescriptor {
	uint32_t numBindings;
	pVgeBinding bindings;
	pVulkanBuffer uniformBuffer;
	VkDescriptorSet descriptorSet;
	pVgePipelineGraphics pipeline;
} vgeDescriptor, *pVgeDescriptor;

pVgeDescriptor createVgeDescriptorSet(pVulkanContext context, pVgeDescriptorInfo info);
void *mapUniformBindingData(pVulkanContext context, pVgeDescriptor descriptors, uint64_t binding, uint64_t offset, uint64_t size);
void unmapUniformBindingData(pVulkanContext context, pVgeDescriptor descriptors, uint64_t binding);

void updateUniformBinding(pVulkanContext context, pVgeDescriptor descriptors,uint64_t binding);

#endif // VGE_MODELDESCRIPTORS_H
