//
// Created by fabio on 7/22/2025.
//

#include "vulkanTexture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void transitionImageLayout(pVulkanContext context, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommand(context);

	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.srcAccessMask = 0,
		.dstAccessMask = 0
	};

	VkPipelineStageFlags sourceStage = 0;
	VkPipelineStageFlags destinationStage = 0;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		printf("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	endSingleTimeCommand(context, commandBuffer, context->queues[0]);
}

void copyBufferToImage(pVulkanContext context, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = beginSingleTimeCommand(context);

	VkBufferImageCopy region = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.baseArrayLayer = 0,
		.imageSubresource.layerCount = 1,
		.imageOffset = {0, 0, 0},
		.imageExtent = {
			width,
			height,
			1
		}
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommand(context, commandBuffer, context->queues[0]);
}

void createImage(pVulkanContext context, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory) {
	VkImageCreateInfo imageInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.extent.width = width,
		.extent.height = height,
		.extent.depth = 1,
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = format,
		.tiling = tiling,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage = usage,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	if (vkCreateImage(context->device, &imageInfo, nullptr, image) != VK_SUCCESS) {
		printf("failed to create image!\n");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(context->device, *image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memRequirements.size,
		.memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties),
	};

	if (vkAllocateMemory(context->device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS) {
		printf("failed to allocate image memory!\n");
	}

	vkBindImageMemory(context->device, *image, *imageMemory, 0);
}

void initImageView(pVulkanContext context, pVgeVulkanTexture texture, VkFormat format) {
	VkImageSubresourceRange subresourceRange = {
		.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1,
	};

	VkImageViewCreateInfo viewInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = texture->image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = subresourceRange,
		.components = 0,
		.flags = 0
	};

	if (vkCreateImageView(context->device, &viewInfo, nullptr, &texture->imageView)) {
		printf("failed to create image view!\n");
	}
}

void initSampler(pVulkanContext context, pVgeVulkanTexture texture) {
	VkSamplerCreateInfo samplerInfo = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		.anisotropyEnable = VK_TRUE,
		.maxAnisotropy = context->physicalDeviceProperties->maxAnisotropy,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.unnormalizedCoordinates = VK_FALSE
	};

	if (vkCreateSampler(context->device, &samplerInfo, nullptr, &texture->sampler)) {
		printf("failed to create sampler!\n");
	}
}

pVgeVulkanTexture createVgeVulkanTexture(pVulkanContext context, const char *path) {
	int texWidth, texHeight;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc* pixels = stbi_load(path, &texWidth, &texHeight, nullptr, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight;

	if (!pixels) {
		printf("failed to load texture image!\n");
	}

	pVulkanBuffer staging = createStagingBuffer(context, imageSize, 4, pixels);

	stbi_image_free(pixels);

	pVgeVulkanTexture texture = calloc(sizeof(pVgeVulkanTexture), 1);

	createImage(
		context,
		texWidth,
		texHeight,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		&texture->image,
		&texture->memory
	);

	transitionImageLayout(context, texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(context, staging->buffer, texture->image, texWidth, texHeight);
	transitionImageLayout(context, texture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	initImageView(context, texture, VK_FORMAT_R8G8B8A8_SRGB);
	initSampler(context, texture);

	destroyBuffer(context, staging);
	return texture;
}

void destroyVulkanTexture(pVulkanContext context, pVgeVulkanTexture texture) {
	vkDestroySampler(context->device, texture->sampler, nullptr);
	vkDestroyImageView(context->device, texture->imageView, nullptr);
	vkFreeMemory(context->device, texture->memory, nullptr);
	vkDestroyImage(context->device, texture->image, nullptr);
}