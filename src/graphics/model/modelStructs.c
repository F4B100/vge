//
// Created by fabio on 6/29/2025.
//

#include "modelStructs.h"
#include "../vulkan/vulkanBuffer.h"

void updateDescriptorSet(pVulkanContext context, pVulkanBuffer uniformBuffer, VkDescriptorSet descriptorSet) {
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer->buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = uniformBuffer->size;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(context->device, 1, &descriptorWrite, 0, nullptr);
};

VkDescriptorSet createUniformDescriptorSet(vulkanContext *context, pVulkanBuffer uniformBuffer, pVgePipelineGraphics pipeline) {
	VkDescriptorSet descriptorSet = nullptr;

	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = pipeline->descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &pipeline->descriptorSetLayout,
	};

	if (vkAllocateDescriptorSets(context->device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
		printf("failed to allocate descriptor sets!");
	}

	updateDescriptorSet(context, uniformBuffer, descriptorSet);
	
	return descriptorSet;
}

pVgeModel createVgeModel(pVulkanContext context, pVgeIndexInfo indexInfo, pVgeVertexInfo vertexInfo, pVgeUniformInfo uniformInfo, pVgePipelineGraphics pipeline) {
	pVgeModel result = malloc(sizeof(vgeModel));

	result->uniformBuffer = malloc(sizeof(vgeUniformBuffer));

	result->indexInfo = indexInfo;
	result->vertexInfo = vertexInfo;
	result->uniformInfo = uniformInfo;

	result->vertexBuffer = createVulkanVertexBufferFromInfo(context,vertexInfo);
	result->indexBuffer = createVulkanIndexBufferFromInfo(context,indexInfo);
	result->uniformBuffer->uniformBuffer = createVulkanUniformBufferFromInfo(context,uniformInfo);

	result->uniformBuffer->descriptorSet = createUniformDescriptorSet(context, result->uniformBuffer->uniformBuffer, pipeline);

	return result;
}

void destroyVgeModel(pVulkanContext context, pVgeModel model) {
	destroyBuffer(context, model->vertexBuffer);
	destroyBuffer(context, model->indexBuffer);
	destroyBuffer(context, model->uniformBuffer->uniformBuffer);
	free(model);
}
