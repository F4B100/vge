//
// Created by fabio on 6/29/2025.
//

#include "modelStructs.h"

pVgeModel createVgeModel(pVulkanContext context, pVgeModelCreateInfo modelCreateInfo) {
	pVgeModel result = calloc(sizeof(vgeModel), 1);

	memcpy(&result->indexInfo, modelCreateInfo->indexInfo, sizeof(vgeIndexInfo));
	result->indexBuffer = createVulkanIndexBufferFromInfo(context, result->indexInfo);

	memcpy(&result->vertexInfo, modelCreateInfo->vertexInfo, sizeof(vgeVertexInfo));
	result->vertexBuffer = createVulkanVertexBufferFromInfo(context, result->vertexInfo);

	return result;
}

void destroyVgeModel(pVulkanContext context, pVgeModel model) {
	if (!model) return;
	destroyBuffer(context, model->vertexBuffer);
	destroyBuffer(context, model->indexBuffer);
	free(model);
}
