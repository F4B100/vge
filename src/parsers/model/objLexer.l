%{
    #include "objParser.tab.h"
%}

%option noyywrap
%option reentrant
%option bison-bridge
%option prefix="calc"

%%

"o" {return OBJECT_NAME;}
"vn" {return VERTEX_NORMAL;}
"vt" {return VERTEX_TEXTURE;}
"v" {return VERTEX;}
"s" {return SHADING;}
"usemtl" {return USE_MATERIAL;}
"mtllib" {return MTL_NAME;}
"f" {return FACE;}
"-"?[0-9]*"."[0-9]* {yylval.f = atof(yytext);return FLOAT;}
[a-zA-z0-9]".mtl" {yyval.path = yytex;return MTL_PATH;}
([^ \n\t])* {yyval.str = yytex;return STRING;}
" "



%%