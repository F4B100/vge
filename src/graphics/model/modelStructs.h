//
// Created by fabio on 6/29/2025.
//

#ifndef MODELSTRUCTS_H
#define MODELSTRUCTS_H
#include <stdint.h>
#include "../vulkan/vulkanPipeline.h"
#include "../vulkan/vulkanBuffer.h"
#include "../vulkan/vulkanTexture.h"

typedef struct VgeVertexInfo {
	uint64_t numVertices;
	uint32_t sizeVertex;
	void * data;
} vgeVertexInfo, *pVgeVertexInfo;

typedef struct VgeIndexInfo {
	uint64_t numIndexes;
	uint32_t sizeIndex;
	void * data;
} vgeIndexInfo, *pVgeIndexInfo;

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

typedef struct VgeModelBindingInfo {
	uint32_t binding;
	uint8_t bindingType;
	vgeTextureBindingInfo textureInfo;
	vgeUniformBindingInfo uniformInfo;
} vgeModelBindingInfo, *pVgeModelBindingInfo;

typedef struct VgeModelCreateInfo{
	pVgeVertexInfo vertexInfo;
	pVgeIndexInfo indexInfo;
	uint32_t numBindings;
	pVgeModelBindingInfo bindings;
	pVgePipelineGraphics pipeline;
} vgeModelCreateInfo, *pVgeModelCreateInfo;

typedef struct VgeUniformBinding {
	uint64_t size;
	uint64_t offset;
	pVulkanBuffer buffer;
} vgeUniformBinding, *pVgeUniformBinding;

typedef struct VgeTextureBinding {
	uint8_t option;
	pVgeVulkanTexture texture;
} vgeTextureBinding, *pVgeTextureBinding;

typedef struct VgeModelBinding {
	uint32_t binding;
	uint8_t bindingType;
	vgeTextureBinding texture;
	vgeUniformBinding uniform;
} vgeModelBinding, *pVgeModelBinding;

typedef struct VgeModel {
	vgeVertexInfo vertexInfo;
	pVulkanBuffer vertexBuffer;
	vgeIndexInfo indexInfo;
	pVulkanBuffer indexBuffer;
	uint32_t numBindings;
	pVgeModelBinding bindings;
	pVulkanBuffer uniformBuffer;
	VkDescriptorSet descriptorSet;
	pVgePipelineGraphics pipeline;
} vgeModel, *pVgeModel;

pVgeModel createVgeModel(pVulkanContext context, pVgeModelCreateInfo modelCreateInfo);
void destroyVgeModel(pVulkanContext context, pVgeModel model);

void *mapUniformBindingData(pVulkanContext context,pVgeModel model, uint64_t binding, uint64_t offset, uint64_t size);
void unmapUniformBindingData(pVulkanContext context, pVgeModel model, uint64_t binding);

void updateUniformBinding(pVulkanContext context, pVgeModel model,uint64_t binding);

#endif //MODELSTRUCTS_H
