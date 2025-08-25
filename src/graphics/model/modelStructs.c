//
// Created by fabio on 6/29/2025.
//

#include "modelStructs.h"
#include "../vulkan/vulkanBuffer.h"

void * mapUniformBindingData(pVulkanContext context,pVgeModel model, uint64_t binding, uint64_t offset, uint64_t size) {
	if (binding >= model->numBindings || model->bindings[binding].bindingType != VGE_BINDING_TYPE_UNIFORM_BUFFER) {
		return nullptr;
	}
	uint64_t actualSize;
	if (size == 0) {
		actualSize = model->bindings[binding].uniform.size - offset;
	} else {
		actualSize = size;
	}

	void *toReturn;
	vkMapMemory(
		context->device,
		model->bindings[binding].uniform.buffer->bufferMemory,
		model->bindings[binding].uniform.offset + offset,
		actualSize,
		0,
		&toReturn
	);
	return toReturn;
}

void unmapUniformBindingData(pVulkanContext context, pVgeModel model, uint64_t binding) {
	vkUnmapMemory(context->device, model->bindings[binding].uniform.buffer->bufferMemory);
}



void updateUniformBinding(pVulkanContext context, pVgeModel model,uint64_t binding) {
	if (binding >= model->numBindings || model->bindings[binding].bindingType != VGE_BINDING_TYPE_UNIFORM_BUFFER) {
		return;
	}

	VkDescriptorBufferInfo bufferInfo = {
		.buffer = model->bindings[binding].uniform.buffer->buffer,
		.offset = model->bindings[binding].uniform.offset,
		.range = model->bindings[binding].uniform.buffer->size
	};
	VkWriteDescriptorSet descriptorWrite = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = model->descriptorSet,
			.dstBinding = binding,
			.dstArrayElement = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.pBufferInfo = &bufferInfo,
			.pImageInfo = nullptr,
			.pTexelBufferView = nullptr
	};
	vkUpdateDescriptorSets(context->device, 1, &descriptorWrite, 0, nullptr);
};

void updateTextureBinding(pVulkanContext context, pVgeModel model, uint64_t binding, pVgeVulkanTexture texture) {
	if (binding >= model->numBindings || model->bindings[binding].bindingType != VGE_BINDING_TYPE_TEXTURE) {
		return;
	}
	VkDescriptorImageInfo imageInfo = {
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.imageView = texture->imageView,
		.sampler = texture->sampler
	};
	VkWriteDescriptorSet descriptorWrite = {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = NULL,
		.dstSet = model->descriptorSet,
		.dstBinding = binding,
		.dstArrayElement = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = 1,
		.pBufferInfo = nullptr,
		.pImageInfo = &imageInfo,
		.pTexelBufferView = nullptr
	};
	vkUpdateDescriptorSets(context->device, 1, &descriptorWrite, 0, nullptr);
}

VkDescriptorSet createDescriptorSet(vulkanContext *context, pVgePipelineGraphics pipeline) {
	VkDescriptorSet descriptorSet = nullptr;

	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorPool = pipeline->descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &pipeline->descriptorSetLayout,
	};

	if (vkAllocateDescriptorSets(context->device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		printf("failed to allocate descriptor sets!");
		return nullptr;
	}
	return descriptorSet;
}


pVgeModel createVgeModel(pVulkanContext context, pVgeModelCreateInfo modelCreateInfo) {
	pVgeModel result = calloc(sizeof(vgeModel), 1);

	memcpy(&result->indexInfo, modelCreateInfo->indexInfo, sizeof(vgeIndexInfo));
	result->indexBuffer = createVulkanIndexBufferFromInfo(context, result->indexInfo);

	memcpy(&result->vertexInfo, modelCreateInfo->vertexInfo, sizeof(vgeVertexInfo));
	result->vertexBuffer = createVulkanVertexBufferFromInfo(context, result->vertexInfo);

	result->descriptorSet = createDescriptorSet(
		context,
		modelCreateInfo->pipeline
	);

	result->numBindings = modelCreateInfo->numBindings;
	result->bindings = calloc(result->numBindings, sizeof(vgeModelBinding));

	uint64_t sizeNewUniform = 0;

	for (int i = 0; i < result->numBindings; ++i) {
		switch (modelCreateInfo->bindings[i].bindingType) {
			case VGE_BINDING_TYPE_UNIFORM_BUFFER:
				if (modelCreateInfo->bindings[i].uniformInfo.option == VGE_UNIFORM_CREATE_BUFFER) {
					sizeNewUniform += modelCreateInfo->bindings[i].uniformInfo.sizeUniform;
				}
				break;
			default:
				break;
		}
	}

	result->uniformBuffer = initBuffer(
		context,
		sizeNewUniform,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);

	uint64_t offsetUniform = 0;
	for (int i = 0; i < result->numBindings; ++i) {
		switch (modelCreateInfo->bindings[i].bindingType) {
			case VGE_BINDING_TYPE_UNIFORM_BUFFER:
				result->bindings[i].bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER;
				result->bindings[i].binding = i;
				result->bindings[i].uniform.size = modelCreateInfo->bindings[i].uniformInfo.sizeUniform;
				if (modelCreateInfo->bindings[i].uniformInfo.option == VGE_UNIFORM_CREATE_BUFFER) {
					result->bindings[i].uniform.offset = offsetUniform;
					result->bindings[i].uniform.buffer = result->uniformBuffer;
					offsetUniform += modelCreateInfo->bindings[i].uniformInfo.sizeUniform;
				} else {
					result->bindings[i].uniform.offset = modelCreateInfo->bindings[i].uniformInfo.offsetUniform;
					result->bindings[i].uniform.buffer = modelCreateInfo->bindings[i].uniformInfo.buffer;
				}
				break;
			case VGE_BINDING_TYPE_TEXTURE:
				result->bindings[i].bindingType = VGE_BINDING_TYPE_TEXTURE;
				result->bindings[i].binding = i;
				if (modelCreateInfo->bindings[i].textureInfo.option == VGE_TEXTURE_INITIALIZED) {
					result->bindings[i].texture.texture = modelCreateInfo->bindings[i].textureInfo.texture;
				} else {
					result->bindings[i].texture.texture = createVgeVulkanTexture(context, modelCreateInfo->bindings[i].textureInfo.TexturePath);
				}
				result->bindings[i].texture.option = modelCreateInfo->bindings[i].textureInfo.option;
				updateTextureBinding(context, result, i, result->bindings[i].texture.texture);
				break;
			default:
				break;
		}
	}

	result->pipeline = modelCreateInfo->pipeline;

	return result;
}

void destroyVgeModel(pVulkanContext context, pVgeModel model) {
	if (!model) return;
	destroyBuffer(context, model->vertexBuffer);
	destroyBuffer(context, model->indexBuffer);
	destroyBuffer(context, model->uniformBuffer);
	for (int i = 0; i < model->numBindings; ++i) {
		if (model->bindings[i].bindingType == VGE_BINDING_TYPE_TEXTURE) {
			if (model->bindings[i].texture.option == VGE_TEXTURE_NOT_INITIALIZED) {
				destroyVulkanTexture(context, model->bindings[i].texture.texture);
			}
		}
	}
	vkFreeDescriptorSets(context->device, model->pipeline->descriptorPool, 1, &model->descriptorSet);
	free(model->bindings);
	free(model);
}
