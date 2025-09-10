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
    #include <string.h>

    typedef void* yyscan_t;
    void objerror(parserContext *context, yyscan_t scanner, const char *err);
    int yywrap(void);
    #define yylex objlex
    extern int yylex(YYSTYPE * yylval_param , yyscan_t yyscanner);
%}

%token OBJECT_NAME VERTEX_NORMAL VERTEX_TEXTURE VERTEX SHADING USE_MATERIAL MTL_NAME FACE SLASH
%token <f> NUMBER
%token <path> MTL_PATH
%token <str> STRING
%token <i> INTEGER

%start input

%%

input:
    |line input
    ;

line:
    VERTEX NUMBER NUMBER NUMBER
    {
        vec3 vert = {
            $2,
            $3,
            $4
        };
        vgeVectorAppend(context->model.vertices, vert);
    }
    | VERTEX_NORMAL NUMBER NUMBER NUMBER
    {
        vec3 norm = {
            $2,
            $3,
            $4
        };
        vgeVectorAppend(context->model.normals, norm);
    }
    | VERTEX_TEXTURE NUMBER NUMBER
    {
        vec2 tex = {
            $2,
            $3
        };
        vgeVectorAppend(context->model.textures, tex);
    }
    | OBJECT_NAME STRING
    {
        pObjOffsets offset = vgeVectorAppendEmpty(context->offsets);
        offset->name = $2;
        offset->vertexOffset = context->model.vertices->numElements;
        offset->indexOffset = context->model.indices->numElements;
    }
    | MTL_NAME MTL_PATH
    {
        free($2);
    }
    | USE_MATERIAL STRING
    {
        free($2);
    }
    | SHADING INTEGER
    {
    }
    | FACE vertices
    {
        pObjModelInfo model = &context->model;

        pObjFace face = nullptr;
        for (uint64_t i = 0; i < vgeVectorGetSize(context->faceIndices); i++) {
            int *indices = vgeVectorGetElement(context->faceIndices, i);

            float verts[3];
            float tex[2];
            float norm[3];

            if(indices[0] > 0) {
                float *vertsInfo = vgeVectorGetElement(model->vertices, indices[0] - 1);
                verts[0] = vertsInfo[0];
                verts[1] = vertsInfo[1];
                verts[2] = vertsInfo[2];
            } else {
                verts[0] = 0.0f;
                verts[1] = 0.0f;
                verts[2] = 0.0f;
            }
            if(indices[1] > 0) {
                float *texInfo = vgeVectorGetElement(model->textures, indices[1] - 1);
                tex[0] = texInfo[0];
                tex[1] = texInfo[1];
            } else {
                tex[0] = 0.0f;
                tex[1] = 0.0f;
            }
            if(indices[2] > 0) {
                float *normInfo = vgeVectorGetElement(model->normals, indices[2] - 1);
                norm[0] = normInfo[0];
                norm[1] = normInfo[1];
                norm[2] = normInfo[2];
            } else {
                norm[0] = 0.0f;
                norm[1] = 0.0f;
                norm[2] = 0.0f;
            }

            face = vgeVectorAppendEmpty(model->faces);
            memcpy(&face->faceVertex, verts, sizeof(vec3));
            memcpy(&face->faceTexture, tex, sizeof(vec2));
            memcpy(&face->faceNormal, norm, sizeof(vec3));
        }

        uint32_t index = 1;
        uint32_t realIndex = 0;

        while(index < vgeVectorGetSize(context->faceIndices) - 1) {
            realIndex = context->currentIndex;
            vgeVectorAppend(model->indices, &realIndex);
            realIndex = context->currentIndex + index;
            vgeVectorAppend(model->indices, &realIndex);
            realIndex++;
            vgeVectorAppend(model->indices, &realIndex);
            index ++;
        }

        context->currentIndex += vgeVectorGetSize(context->faceIndices);

        vgeVectorFree(context->faceIndices);
        context->faceIndices = vgeVectorInit(sizeof(ivec3));
    }
;

vertices:
    |vertex vertices
;

vertex:
    INTEGER
    {
        ivec3 vertexIndices = {
            $1,
            -1,
            -1
        };
        vgeVectorAppend(context->faceIndices, &vertexIndices);
    }
    |INTEGER SLASH INTEGER
    {
        ivec3 vertexIndices = {
            $1,
            $3,
            -1
        };
        vgeVectorAppend(context->faceIndices, &vertexIndices);
    }
    |INTEGER SLASH SLASH INTEGER
    {
        ivec3 vertexIndices = {
            $1,
            -1,
            $4
        };
        vgeVectorAppend(context->faceIndices, &vertexIndices);
    }
    |INTEGER SLASH INTEGER SLASH INTEGER
    {
        ivec3 vertexIndices = {
            $1,
            $3,
            $5
        };
        vgeVectorAppend(context->faceIndices, &vertexIndices);
    }

%%

void objerror(parserContext *context, yyscan_t scanner, const char *err) {
    fprintf(stderr, "Lexer error:%s\n", err);
}

int yywrap(void) {
  return 1;
}
