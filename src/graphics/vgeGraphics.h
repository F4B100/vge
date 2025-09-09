//
// Created by fabio on 19-06-2025.
//

#ifndef VGEGRAPHICS_H
#define VGEGRAPHICS_H

#ifdef VGE_USE_VULKAN
#include "vulkan/vulkanDefs.h"
#include "vulkan/vulkanInit.h"
#include "vulkan/vulkanPipeline.h"
#include "vulkan/vulkanQueues.h"
#include "vulkan/vulkanLogicalDevice.h"
#include "vulkan/vulkanPhysicalDevice.h"
#include "vulkan/vulkanSwapChain.h"
#include "vulkan/vulkanRender.h"
#include "vulkan/vulkanBuffer.h"
#include "model/modelStructs.h"
#include "model/modelDescriptors.h"
#endif

#endif //VGEGRAPHICS_H
