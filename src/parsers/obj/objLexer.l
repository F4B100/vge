%option noyywrap
%option reentrant
%option bison-bridge
%option prefix="obj"

%{
#define YYSTYPE OBJSTYPE
#include "objParser.tab.h"
%}

%%

"o"                         {return OBJECT_NAME;}
"vn"                        {return VERTEX_NORMAL;}
"vt"                        {return VERTEX_TEXTURE;}
"v"                         {return VERTEX;}
"s"                         {return SHADING;}
"usemtl"                    {return USE_MATERIAL;}
"mtllib"                    {return MTL_NAME;}
"f"                         {return FACE;}
"/"                         {return SLASH;}
[-+]?[0-9]*"."[0-9]*        {yylval_param->f = atof(yytext);return NUMBER;}
[-+]?[0-9]*                 {yylval_param->i = atoi(yytext);return INTEGER;}
[a-zA-Z0-9_.\/\\-]+\.mtl    {yylval_param->path = strdup(yytext);return MTL_PATH;}
([^ \n\t/])*                 {yylval_param->str = strdup(yytext);return STRING;}
"\n"
#[^\n]*
" "

%%