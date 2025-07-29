%{
  #define _GNU_SOURCE
  #include <stdio.h>


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

%%

input: /* EMPTY */
    |line '\n' input

line:




%%