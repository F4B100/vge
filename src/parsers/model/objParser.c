//
// Created by fabio on 8/6/2025.
//

#include "objParser.h"
#include "objParser.tab.h"
#define YYSTYPE OBJSTYPE
#include "objLexer.h"

pObjModel parseObjFile(char * filename) {
	pObjModel model = malloc(sizeof(objModel));
	if (!model) {
		return nullptr;
	}
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("Error opening file\n");
		free(model);
		return nullptr;
	}

	yyscan_t scanner = {};

	if (objlex_init(&scanner)) {
		printf("Error initializing parser for file: %s\n", filename);
		fclose(file);
		free(model);
		return nullptr;
	}
	objset_in(file, scanner);

	parserContext context = {};

	context.model = malloc(sizeof(objModel));

	int result = objparse(&context, scanner);

	if (result) {
		printf("Error parsing for file: %s\n", filename);
	}

	objlex_destroy(scanner);
	fclose(file);

	if (result != 0) {
		fprintf(stderr, "Parse failed.\n");
		free(model);
		return NULL;
	}

	model = context.model;

	return model;
}
