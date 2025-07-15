//
// Created by fabio on 7/14/2025.
//

#include "defaultModels.h"

#define NUM_TRIANGLE_VERTICES 3
float triangleVertices[] = {
	0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f,
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f
};

#define NUM_TRIANGLE_INDICES 3
uint32_t triangleIndices[] = {
	0,
	1,
	2
};

struct triangleUniforms {
	vec2 position;
};

struct triangleUniforms triangleUniformInfo = {
	.position = {0.5f, 0.5f}
};

vgeVertexInfo triangleInfoVertex = {
	.data = triangleVertices,
	.numVertices = NUM_TRIANGLE_VERTICES,
	.sizeVertex = sizeof(triangleVertices) / NUM_TRIANGLE_VERTICES
};

vgeIndexInfo triangleInfoIndex = {
	.data = triangleIndices,
	.numIndexes = NUM_TRIANGLE_INDICES,
	.sizeIndex = sizeof(triangleIndices) / NUM_TRIANGLE_INDICES
};

vgeUniformInfo uniformInfo = {
	.sizeUniform = sizeof(struct triangleUniforms),
	.data = &triangleUniformInfo
};

pVgeModel createDefaultModel(pVulkanContext context, pVgePipelineGraphics pipeline, uint32_t model) {
	pVgeModel vgeModel = nullptr;
	switch (model) {
		case VGE_MODEL_TRIANGLE:
			vgeModel = createVgeModel(context, &triangleInfoIndex,&triangleInfoVertex, &uniformInfo, pipeline);
			break;
		case VGE_MODEL_CUBE:
			break;
		default:
			return nullptr;
			break;
	}
	return vgeModel;
}

