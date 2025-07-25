//
// Created by fabio on 6/29/2025.
//

#include "modelStructs.h"
#include "../vulkan/vulkanBuffer.h"

void updateDescriptorSet(pVulkanContext context, pVulkanBuffer uniformBuffer, pVgeVulkanTexture texture, VkDescriptorSet descriptorSet) {
	VkDescriptorBufferInfo bufferInfo = {
		.buffer = uniformBuffer->buffer,
		.offset = 0,
		.range = uniformBuffer->size
	};
	VkDescriptorImageInfo imageInfo = {
		.imageView = texture->imageView,
		.sampler = texture->sampler,
		.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	};

	VkWriteDescriptorSet descriptorWrite[2] = {
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = descriptorSet,
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.pBufferInfo = &bufferInfo,
			.pImageInfo = nullptr,
			.pTexelBufferView = nullptr
		},
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = NULL,
			.dstSet = descriptorSet,
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1,
			.pBufferInfo = nullptr,
			.pImageInfo = &imageInfo,
			.pTexelBufferView = nullptr
		}
	};
	vkUpdateDescriptorSets(context->device, 2, descriptorWrite, 0, nullptr);
};

VkDescriptorSet createDescriptorSet(vulkanContext *context, pVulkanBuffer uniformBuffer, pVgeVulkanTexture texture, pVgePipelineGraphics pipeline) {
	VkDescriptorSet descriptorSet = nullptr;

	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = pipeline->descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &pipeline->descriptorSetLayout,
	};

	if (vkAllocateDescriptorSets(context->device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		printf("failed to allocate descriptor sets!");
		return nullptr;
	}

	updateDescriptorSet(context, uniformBuffer, texture, descriptorSet);
	
	return descriptorSet;
}

pVgeModel createVgeModel(pVulkanContext context, pVgeIndexInfo indexInfo, pVgeVertexInfo vertexInfo, pVgeUniformInfo uniformInfo, pVgePipelineGraphics pipeline) {
	pVgeModel result = malloc(sizeof(vgeModel));

	result->indexInfo = indexInfo;
	result->vertexInfo = vertexInfo;
	result->uniformInfo = uniformInfo;

	result->vertexBuffer = createVulkanVertexBufferFromInfo(context,vertexInfo);
	result->indexBuffer = createVulkanIndexBufferFromInfo(context,indexInfo);
	result->uniformBuffer = createVulkanUniformBufferFromInfo(context,uniformInfo);

	result->texture = createVgeVulkanTexture(context, "textures/img.png");
	result->descriptorSet = createDescriptorSet(context, result->uniformBuffer, result->texture, pipeline);
	result->pipeline = pipeline;

	return result;
}

void destroyVgeModel(pVulkanContext context, pVgeModel model) {
	destroyBuffer(context, model->vertexBuffer);
	destroyBuffer(context, model->indexBuffer);
	destroyBuffer(context, model->uniformBuffer);
	destroyVulkanTexture(context, model->texture);
	vkFreeDescriptorSets(context->device, model->pipeline->descriptorPool, 1, &model->descriptorSet);
	free(model);
}
