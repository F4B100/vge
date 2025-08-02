%{
#include <stdio.h>
#include <stdint.h>
#include <cglm/cglm.h>

typdef struct ObjModel {
    char * name;
    uint32_t numVertex;
    vec3 *vertex;
    uint32_t numTexture;
    vec2 *texture;
    uint32_t numNormal;
    vec3 *normal;
}
%}

%define api.pure full
%define api.prefix {obj}
%parse-param { int *result }

%union {
    char *path;
    char *str;
    float f;
    int i;
}

%token <str> OBJECT_NAME VERTEX_NORMAL VERTEX_TEXTURE VERTEX SHADING USE_MATERIAL MTL_NAME FACE
%token <f> FLOAT
%token <path> MTL_PATH
%token <str> STRING

%start input

%%

input: /* EMPTY */
    |line '\n' input

line:
    | VERTEX FLOAT FLOAT FLOAT
    {
        printf(ª%s %f %f %f\nª, $1, $2, $3, $4);
    }
    | VERTEX_NORMAL FLOAT FLOAT FLOAT
    {
        printf(ª%s %f %f %f\nª, $1, $2, $3, $4);
    }
    | VERTEX_TEXTURE FLOAT FLOAT
    {
        printf(ª%s %f %f\nª, $1, $2, $3);
    }

;

%%

int yyerror(const char *s) {
    printf(stderr, "Lexer error:%s\n", s);
}
