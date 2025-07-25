//
// Created by fabio on 7/22/2025.
//

#ifndef VULKANTEXTURE_H
#define VULKANTEXTURE_H

#include "vulkanBuffer.h"
#include "vulkanCommands.h"

typedef struct VgeVulkanTexture {
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	VkSampler sampler;
} vgeVulkanTexture, *pVgeVulkanTexture;

pVgeVulkanTexture createVgeVulkanTexture(pVulkanContext context, const char * path);
void destroyVulkanTexture(pVulkanContext context, pVgeVulkanTexture texture);

#endif //VULKANTEXTURE_H
