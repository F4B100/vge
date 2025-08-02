%{
    #include "objParser.tab.h"
%}

%option noyywrap
%option reentrant
%option bison-bridge
%option prefix="obj"

%%

"o" {yylval.str = yytext;return OBJECT_NAME;}
"vn" {yylval.str = yytext;return VERTEX_NORMAL;}
"vt" {yylval.str = yytext;return VERTEX_TEXTURE;}
"v" {yylval.str = yytext;return VERTEX;}
"s" {yylval.str = yytext;return SHADING;}
"usemtl" {yylval.str = yytext;return USE_MATERIAL;}
"mtllib" {yylval.str = yytext;return MTL_NAME;}
"f" {return FACE;}
"-"?[0-9]*"."[0-9]* {yylval.f = atof(yytext);return FLOAT;}
[a-zA-z0-9]".mtl" {yyval.path = yytex;return MTL_PATH;}
([^ \n\t])* {yyval.str = yytex;return STRING;}
" "

%%