//
// Created by fabio on 7/14/2025.
//

#ifndef DEFAULTMODELS_H
#define DEFAULTMODELS_H

#include "modelStructs.h"

typedef uint32_t vgeDefaultModelId;

#define VGE_MODEL_TRIANGLE 0
#define VGE_MODEL_CUBE 1

pVgeModel createDefaultModel(pVulkanContext context, pVgePipelineGraphics pipeline, uint32_t model);

#endif //DEFAULTMODELS_H
