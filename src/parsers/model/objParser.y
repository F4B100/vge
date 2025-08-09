%code requires {
    #include "objParser.h"
    typedef void* yyscan_t;
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
%}

%union {
    char *path;
    char *str;
    float f;
    int i;
}

%token OBJECT_NAME VERTEX_NORMAL VERTEX_TEXTURE VERTEX SHADING USE_MATERIAL MTL_NAME FACE
%token <f> FLOAT
%token <path> MTL_PATH
%token <str> STRING
%token <i> INTEGER

%start input

%%

input: /* EMPTY */
    |line '\n' input
;

line:
    | VERTEX FLOAT FLOAT FLOAT
    {
        printf("vertex: %f %f %f\n", $2, $3, $4);
    }
    | VERTEX_NORMAL FLOAT FLOAT FLOAT
    {
        printf("vertex Normal: %f %f %f\n", $2, $3, $4);
    }
    | VERTEX_TEXTURE FLOAT FLOAT
    {
        printf("vertex Texture: %f %f\n", $2, $3);
    }
    | OBJECT_NAME STRING
    {
        printf("object name: %s\n", $2);
    }
    | MTL_NAME MTL_PATH
    {
        printf("material name: %s\n", $2);
    }
    | USE_MATERIAL MTL_PATH
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
    | FACE face face face
;

face:
    INTEGER '/' INTEGER '/' INTEGER
    {
        printf("%d/%d/%d\n", $1, $3, $5);
    }
    |INTEGER '/' '/' INTEGER
    {
        printf("%d//%d\n", $1, $4);
    }
;

%%

void objerror(parserContext *context, yyscan_t scanner, const char *err) {
    fprintf(stderr, "Lexer error:%s\n", err);
}

int yywrap(void) {
  return 1;
}
