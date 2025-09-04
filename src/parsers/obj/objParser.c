//
// Created by fabio on 8/6/2025.
//

#include "objParser.h"
#include "objParser.tab.h"
#define YYSTYPE OBJSTYPE
#include "objLexer.h"

pVgeModel parseObjFile(pVulkanContext context, pVgePipelineGraphics pipeline, char * filename) {
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening file\n");
		return nullptr;
	}

	yyscan_t scanner = {};

	if (objlex_init(&scanner)) {
		printf("Error initializing parser for file: %s\n", filename);
		fclose(file);
		return nullptr;
	}
	objset_in(file, scanner);

	parserContext parserContext = {};

	parserContext.model = calloc(sizeof(objModel), 1);

	parserContext.model->vertices = vgeVectorInit(sizeof(vec3));
	parserContext.model->textures = vgeVectorInit(sizeof(vec2));
	parserContext.model->normals = vgeVectorInit(sizeof(vec3));
	parserContext.model->indices = vgeVectorInit(sizeof(uint32_t));
	parserContext.model->faces = vgeVectorInit(sizeof(objFace));
    parserContext.faceIndices = vgeVectorInit(sizeof(ivec3));

	int result = objparse(&parserContext, scanner);

	if (result) {
		printf("Error parsing for file: %s\n", filename);
	}

	objlex_destroy(scanner);
	fclose(file);

	if (result != 0) {
		fprintf(stderr, "Parse failed.\n");
		return NULL;
	}

	vgeVectorFree(parserContext.model->vertices);
	vgeVectorFree(parserContext.model->textures);
	vgeVectorFree(parserContext.model->normals);
	vgeVectorFree(parserContext.faceIndices);

	vgeModelCreateInfo info = {};


	vgeModelBindingInfo infoBinding = {
		.binding = 0,
		.bindingType = VGE_BINDING_TYPE_UNIFORM_BUFFER,
		.uniformInfo = {
			.option = VGE_UNIFORM_CREATE_BUFFER,
			.sizeUniform = sizeof(mat4) * 3
		}
	};

	info.numBindings = 1;
	info.bindings = &infoBinding;
	info.pipeline = pipeline;

	vgeVertexInfo vertexInfo = {
		.numVertices = vgeVectorGetSize(parserContext.model->faces),
		.data = vgeVectorGetData(parserContext.model->faces),
		.sizeVertex = vgeVectorGetSizeElement(parserContext.model->faces)
	};

	info.vertexInfo = &vertexInfo;

	vgeIndexInfo indexInfo = {
		.numIndexes = vgeVectorGetSize(parserContext.model->indices),
		.data = vgeVectorGetData(parserContext.model->indices),
		.sizeIndex = vgeVectorGetSizeElement(parserContext.model->indices)
	};

	info.indexInfo = &indexInfo;

	return createVgeModel(context, &info);
}
