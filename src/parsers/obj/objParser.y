%code requires {
    #include "objParser.h"
    #define yylex objlex
    typedef void* yyscan_t;
}

%union {
    char *path;
    char *str;
    float f;
    int i;
}

%define api.pure full
%define api.prefix {obj}
%define parse.error verbose

%parse-param    {parserContext *context}
%parse-param    {yyscan_t scanner}
%lex-param      {yyscan_t scanner}

%{
    #include "objParser.h"

    typedef void* yyscan_t;
    void objerror(parserContext *context, yyscan_t scanner, const char *err);
    int yywrap(void);
    #define yylex objlex
    extern int yylex(YYSTYPE * yylval_param , yyscan_t yyscanner);
%}

%token OBJECT_NAME VERTEX_NORMAL VERTEX_TEXTURE VERTEX SHADING USE_MATERIAL MTL_NAME FACE SLASH
%token <f> FLOAT
%token <path> MTL_PATH
%token <str> STRING
%token <i> INTEGER

%start input

%%

input:
    |line input
    ;

line:
    VERTEX FLOAT FLOAT FLOAT
    {
        vec3 vert = {
            $2,
            $3,
            $4
        };
        vgeVectorAppend(context->model->vertices, vert);
    }
    | VERTEX_NORMAL FLOAT FLOAT FLOAT
    {
        vec3 norm = {
            $2,
            $3,
            $4
        };
        vgeVectorAppend(context->model->normals, norm);
        printf("vertex Normal: %f %f %f\n", $2, $3, $4);
    }
    | VERTEX_TEXTURE FLOAT FLOAT
    {
        vec2 tex = {
            $2,
            $3
        };
        vgeVectorAppend(context->model->textures, tex);
        printf("vertex Texture: %f %f\n", $2, $3);
    }
    | OBJECT_NAME STRING
    {
        context->model->name = $2;
        printf("model name: %s\n", $2);
    }
    | MTL_NAME MTL_PATH
    {
        printf("material name: %s\n", $2);
    }
    | USE_MATERIAL STRING
    {
        printf("use material: %s\n", $2);
    }
    | SHADING INTEGER
    {
        printf("Shading: %d\n", $2);
    }
    | USE_MATERIAL STRING
    {
        printf("usemtl: %s\n", $2);
    }
    | FACE indices
    {
        printf("\n");
    }
;

indices:
    INTEGER SLASH INTEGER SLASH INTEGER
    {
        printf("%d/%d/%d ", $1, $3, $5);
    }
    |INTEGER SLASH SLASH INTEGER
    {
        printf("%d/%d", $1, $4);
    }
;

%%

void objerror(parserContext *context, yyscan_t scanner, const char *err) {
    fprintf(stderr, "Lexer error:%s\n", err);
}

int yywrap(void) {
  return 1;
}
