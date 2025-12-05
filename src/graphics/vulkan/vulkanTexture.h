//
// Created by fabio on 7/22/2025.
//

#ifndef VULKANTEXTURE_H
#define VULKANTEXTURE_H

#include "vulkanDefs.h"
#include "vulkanBuffer.h"
#include "vulkanCommands.h"

pVgeVulkanTexture createVgeVulkanTexture(pVulkanContext context, const char * path);
pVgeDepthBuffer createDepthBuffer(pVulkanContext context, uint32_t width, uint32_t height);
void destroyVulkanTexture(pVulkanContext context, pVgeVulkanTexture texture);

#endif //VULKANTEXTURE_H
