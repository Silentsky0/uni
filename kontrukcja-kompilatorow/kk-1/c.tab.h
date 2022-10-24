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

#ifndef YY_YY_C_TAB_H_INCLUDED
# define YY_YY_C_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KW_CHAR = 258,                 /* KW_CHAR  */
    KW_UNSIGNED = 259,             /* KW_UNSIGNED  */
    KW_SHORT = 260,                /* KW_SHORT  */
    KW_INT = 261,                  /* KW_INT  */
    KW_LONG = 262,                 /* KW_LONG  */
    KW_FLOAT = 263,                /* KW_FLOAT  */
    KW_VOID = 264,                 /* KW_VOID  */
    KW_FOR = 265,                  /* KW_FOR  */
    KW_DOUBLE = 266,               /* KW_DOUBLE  */
    KW_IF = 267,                   /* KW_IF  */
    KW_ELSE = 268,                 /* KW_ELSE  */
    KW_WHILE = 269,                /* KW_WHILE  */
    KW_DO = 270,                   /* KW_DO  */
    KW_STRUCT = 271,               /* KW_STRUCT  */
    INTEGER_CONST = 272,           /* INTEGER_CONST  */
    FLOAT_CONST = 273,             /* FLOAT_CONST  */
    IDENT = 274,                   /* IDENT  */
    STRING_CONST = 275,            /* STRING_CONST  */
    CHARACTER_CONST = 276,         /* CHARACTER_CONST  */
    INC = 277,                     /* INC  */
    LE = 278                       /* LE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 12 "c.y"

  char s[MAX_STR_LEN + 1]; /* pole tekstowe dla nazw itp. */
  int i; /* pole całkowite */
  double d; /* pole zmiennoprzecinkowe */

#line 93 "c.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_C_TAB_H_INCLUDED  */
