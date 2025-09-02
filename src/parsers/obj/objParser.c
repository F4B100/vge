//
// Created by fabio on 8/6/2025.
//

#include "objParser.h"
#include "objParser.tab.h"
#define YYSTYPE OBJSTYPE
#include "objLexer.h"

pObjModel parseObjFile(char * filename) {
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

	parserContext context = {};

	context.model = calloc(sizeof(objModel), 1);

	context.model->vertices = vgeVectorInit(sizeof(vec3));
	context.model->textures = vgeVectorInit(sizeof(vec2));
	context.model->normals = vgeVectorInit(sizeof(vec3));
	context.model->indices = vgeVectorInit(sizeof(uint32_t));
	context.model->faces = vgeVectorInit(sizeof(objFace));

	int result = objparse(&context, scanner);

	if (result) {
		printf("Error parsing for file: %s\n", filename);
	}

	objlex_destroy(scanner);
	fclose(file);

	if (result != 0) {
		fprintf(stderr, "Parse failed.\n");
		return NULL;
	}

	pObjModel model = context.model;
	return model;
}
