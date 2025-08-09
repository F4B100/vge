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
%}

%token OBJECT_NAME VERTEX_NORMAL VERTEX_TEXTURE VERTEX SHADING USE_MATERIAL MTL_NAME FACE SLASH
%token <f> FLOAT
%token <path> MTL_PATH
%token <str> STRING
%token <i> INTEGER

%start line

%%

line:
    | VERTEX FLOAT FLOAT FLOAT line
    {
        printf("vertex: %f %f %f\n", $2, $3, $4);
    }
    | VERTEX_NORMAL FLOAT FLOAT FLOAT line
    {
        printf("vertex Normal: %f %f %f\n", $2, $3, $4);
    }
    | VERTEX_TEXTURE FLOAT FLOAT line
    {
        printf("vertex Texture: %f %f\n", $2, $3);
    }
    | OBJECT_NAME STRING line
    {
        printf("object name: %s\n", $2);
    }
    | MTL_NAME MTL_PATH line
    {
        printf("material name: %s\n", $2);
    }
    | USE_MATERIAL MTL_PATH line
    {
        printf("use material: %s\n", $2);
    }
    | SHADING INTEGER line
    {
        printf("Shading: %d\n", $2);
    }
    | USE_MATERIAL STRING line
    {
        printf("usemtl: %s\n", $2);
    }
    | FACE indices indices indices indices line
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
