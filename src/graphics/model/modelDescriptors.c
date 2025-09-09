//
// Created by fabio on 9/7/2025.
//

#include "modelDescriptors.h"

void * mapUniformBindingData(pVulkanContext context, pVgeDescriptor descriptor, uint64_t binding, uint64_t offset, uint64_t size) {
	if (binding >= descriptor->numBindings || descriptor->bindings[binding].bindingType != VGE_BINDING_TYPE_UNIFORM_BUFFER) {
		return nullptr;
	}
	uint64_t actualSize;
	if (size == 0) {
		actualSize = descriptor->bindings[binding].uniform.size - offset;
	} else {
		actualSize = size;
	}

	void *toReturn;
	vkMapMemory(
		context->device,
		descriptor->bindings[binding].uniform.buffer->bufferMemory,
		descriptor->bindings[binding].uniform.offset + offset,
		actualSize,
		0,
		&toReturn
	);
	return toReturn;
}

void unmapUniformBindingData(pVulkanContext context, pVgeDescriptor descriptor, uint64_t binding) {
	vkUnmapMemory(context->device, descriptor->bindings[binding].uniform.buffer->bufferMemory);
}



void updateUniformBinding(pVulkanContext context, pVgeDescriptor descriptor,uint64_t binding) {
	if (binding >= descriptor->numBindings || descriptor->bindings[binding].bindingType != VGE_BINDING_TYPE_UNIFORM_BUFFER) {
		return;
	}

	VkDescriptorBufferInfo bufferInfo = {
		.buffer = descriptor->bindings[binding].uniform.buffer->buffer,
		.offset = descriptor->bindings[binding].uniform.offset,
		.range = descriptor->bindings[binding].uniform.buffer->size
	};
	VkWriteDescriptorSet descriptorWrite = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = nullptr,
			.dstSet = descriptor->descriptorSet,
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

void updateTextureBinding(pVulkanContext context, pVgeDescriptor descriptor, uint64_t binding, pVgeVulkanTexture texture) {
	if (binding >= descriptor->numBindings || descriptor->bindings[binding].bindingType != VGE_BINDING_TYPE_TEXTURE) {
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
		.dstSet = descriptor->descriptorSet,
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

pVgeDescriptor createVgeDescriptorSet(pVulkanContext context, pVgeDescriptorInfo info) {
	pVgeDescriptor result = calloc(1, sizeof(pVgeDescriptor));
	result->descriptorSet = createDescriptorSet(
		context,
		info->pipeline
	);

	result->numBindings = info->numBindings;
	result->bindings = calloc(result->numBindings, sizeof(vgeBinding));

	uint64_t sizeNewUniform = 0;

	for (int i = 0; i < result->numBindings; ++i) {
		switch (info->bindings[i].bindingType) {
			case VGE_BINDING_TYPE_UNIFORM_BUFFER:
				if (info->bindings[i].uniformInfo.option == VGE_UNIFORM_CREATE_BUFFER) {
					sizeNewUniform += info->bindings[i].uniformInfo.sizeUniform;
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
		switch (info->bindings[i].bindingType) {
			case VGE_BINDING_TYPE_UNIFORM_BUFFER:
				result->bindings[i].bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER;
				result->bindings[i].binding = i;
				result->bindings[i].uniform.size = info->bindings[i].uniformInfo.sizeUniform;
				if (info->bindings[i].uniformInfo.option == VGE_UNIFORM_CREATE_BUFFER) {
					result->bindings[i].uniform.offset = offsetUniform;
					result->bindings[i].uniform.buffer = result->uniformBuffer;
					offsetUniform += info->bindings[i].uniformInfo.sizeUniform;
				} else {
					result->bindings[i].uniform.offset = info->bindings[i].uniformInfo.offsetUniform;
					result->bindings[i].uniform.buffer = info->bindings[i].uniformInfo.buffer;
				}
				break;
			case VGE_BINDING_TYPE_TEXTURE:
				result->bindings[i].bindingType = VGE_BINDING_TYPE_TEXTURE;
				result->bindings[i].binding = i;
				if (info->bindings[i].textureInfo.option == VGE_TEXTURE_INITIALIZED) {
					result->bindings[i].texture.texture = info->bindings[i].textureInfo.texture;
				} else {
					result->bindings[i].texture.texture = createVgeVulkanTexture(context, info->bindings[i].textureInfo.TexturePath);
				}
				result->bindings[i].texture.option = info->bindings[i].textureInfo.option;
				updateTextureBinding(context, result, i, result->bindings[i].texture.texture);
				break;
			default:
				break;
		}
	}

	result->pipeline = info->pipeline;
	return result;
}
