/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_OBJ_OBJPARSER_TAB_H_INCLUDED
# define YY_OBJ_OBJPARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef OBJDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define OBJDEBUG 1
#  else
#   define OBJDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define OBJDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined OBJDEBUG */
#if OBJDEBUG
extern int objdebug;
#endif
/* "%code requires" blocks.  */
#line 1 "objParser.y"

    #include "objParser.h"
    #define yylex objlex
    typedef void* yyscan_t;

#line 63 "objParser.tab.h"

/* Token kinds.  */
#ifndef OBJTOKENTYPE
# define OBJTOKENTYPE
  enum objtokentype
  {
    OBJEMPTY = -2,
    OBJEOF = 0,                    /* "end of file"  */
    OBJerror = 256,                /* error  */
    OBJUNDEF = 257,                /* "invalid token"  */
    OBJECT_NAME = 258,             /* OBJECT_NAME  */
    VERTEX_NORMAL = 259,           /* VERTEX_NORMAL  */
    VERTEX_TEXTURE = 260,          /* VERTEX_TEXTURE  */
    VERTEX = 261,                  /* VERTEX  */
    SHADING = 262,                 /* SHADING  */
    USE_MATERIAL = 263,            /* USE_MATERIAL  */
    MTL_NAME = 264,                /* MTL_NAME  */
    FACE = 265,                    /* FACE  */
    SLASH = 266,                   /* SLASH  */
    NUMBER = 267,                  /* NUMBER  */
    MTL_PATH = 268,                /* MTL_PATH  */
    STRING = 269,                  /* STRING  */
    INTEGER = 270                  /* INTEGER  */
  };
  typedef enum objtokentype objtoken_kind_t;
#endif

/* Value type.  */
#if ! defined OBJSTYPE && ! defined OBJSTYPE_IS_DECLARED
union OBJSTYPE
{
#line 7 "objParser.y"

    char *path;
    char *str;
    float f;
    int i;

#line 102 "objParser.tab.h"

};
typedef union OBJSTYPE OBJSTYPE;
# define OBJSTYPE_IS_TRIVIAL 1
# define OBJSTYPE_IS_DECLARED 1
#endif




int objparse (parserContext *context, yyscan_t scanner);


#endif /* !YY_OBJ_OBJPARSER_TAB_H_INCLUDED  */
