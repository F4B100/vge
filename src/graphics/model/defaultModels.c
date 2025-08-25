//
// Created by fabio on 7/14/2025.
//

#include "defaultModels.h"

struct triangleUniforms {
	mat4 model;
	mat4 view;
	mat4 proj;
};

pVgeModel createDefaultModel(pVulkanContext context, pVgePipelineGraphics pipeline, uint32_t model) {
	pVgeModel vgeModel = nullptr;
	switch (model) {
		case VGE_MODEL_TRIANGLE:

			float triangleVertices[] = {
				-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
				0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
				0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
				-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
			};

			uint32_t triangleIndices[] = {
				0,
				1,
				2,
				2,
				3,
				0
			};

			vgeVertexInfo vertexInfo = {
				.data = triangleVertices,
				.numVertices = 4,
				.sizeVertex = sizeof(triangleVertices) / 4
			};

			vgeIndexInfo indexInfo = {
				.data = triangleIndices,
				.numIndexes = 6,
				.sizeIndex = sizeof(uint32_t)
			};

			vgeModelBindingInfo bindings[2] = {
				{
					.binding = 0,
					.bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER,
					.uniformInfo = {
						.sizeUniform = sizeof(mat4) * 3
					}
				},{
					.binding = 1,
					.bindingType = VGE_BINDING_TYPE_TEXTURE,
					.textureInfo = {
						.option = VGE_TEXTURE_NOT_INITIALIZED,
						.TexturePath = "textures/img.png",
					}
				}
			};

			vgeModelCreateInfo createInfo = {
				.vertexInfo = &vertexInfo,
				.indexInfo = &indexInfo,
				.numBindings = 2,
				.bindings = bindings,
				.pipeline = pipeline
			};

			vgeModel = createVgeModel(context, &createInfo);
			break;
		case VGE_MODEL_CUBE:
			break;
		default:
			return nullptr;
			break;
	}
	return vgeModel;
}

