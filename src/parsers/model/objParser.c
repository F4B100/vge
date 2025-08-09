//
// Created by fabio on 8/6/2025.
//

#include "objParser.h"
#include "objParser.tab.h"
#define YYSTYPE OBJSTYPE
#include "objLexer.h"

extern int yylex_init(yyscan_t* scanner);
extern void yyset_in(FILE * _in_str, yyscan_t yyscanner);
extern int yylex(OBJSTYPE * yylval_param, yyscan_t yyscanner);

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

	int result = objparse(&context, scanner);

	objlex_destroy(scanner);
	fclose(file);

	if (result != 0) {
		fprintf(stderr, "Parse failed.\n");
		free(model);
		return NULL;
	}

	return model;
}
