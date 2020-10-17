%{
#ifdef __MINGW32__
#include_next <unistd.h>
#else
#include <io.h>
#include <process.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "yacc_MaterialScript.hpp"

#define STRING_ALLOC_SIZE       (256)

static char *st_string_literal_buffer = NULL;
static int st_string_literal_buffer_size = 0;
static int st_string_literal_buffer_alloc_size = 0;

static void open_string_literal(void)
{
	st_string_literal_buffer_size = 0;
}

static void add_string_literal(int letter)
{
	if (st_string_literal_buffer_size == st_string_literal_buffer_alloc_size)
	{
		st_string_literal_buffer_alloc_size += STRING_ALLOC_SIZE;
		st_string_literal_buffer = (char*)realloc(st_string_literal_buffer, st_string_literal_buffer_alloc_size);
	}
	st_string_literal_buffer[st_string_literal_buffer_size] = letter;
	st_string_literal_buffer_size++;
}

static void reset_string_literal_buffer(void)
{
	free(st_string_literal_buffer);
	st_string_literal_buffer = NULL;
	st_string_literal_buffer_size = 0;
	st_string_literal_buffer_alloc_size = 0;
}

static char* close_string_literal(void)
{
	char *new_str;

	new_str = (char*)malloc(st_string_literal_buffer_size + 1);

	memcpy(new_str, st_string_literal_buffer, st_string_literal_buffer_size);
	new_str[st_string_literal_buffer_size] = '\0';

	return new_str;
}

extern "C" int yywrap(void)
{
	return 1;
}
%}
%start COMMENT STRING_LITERAL_STATE
%%
<INITIAL>"{"                    return LC;
<INITIAL>"}"                    return RC;

<INITIAL>"material"             return MATERIAL;
<INITIAL>"technique"            return TECHNIQUE;
<INITIAL>"pass"                 return PASS;

<INITIAL>"ambient"              return AMBIENT;
<INITIAL>"diffuse"              return DIFFUSE;
<INITIAL>"specular"             return SPECULAR;
<INITIAL>"emissive"             return EMISSIVE;

<INITIAL>"texture_unit"         return TEXTURE_UNIT;
<INITIAL>"texture"              return TEXTURE;

<INITIAL>[A-Za-z_][A-Za-z_0-9]* {
	yylval.identifier = yytext;
	return IDENTIFIER;
}
<INITIAL>([1-9][0-9]*)|0|([0-9]+\.[0-9]*) {
	double temp;
	sscanf(yytext, "%lf", &temp);
	yylval.double_value = temp;
	return DOUBLE_LITERAL;
}
<INITIAL>\" {
	open_string_literal();
	BEGIN STRING_LITERAL_STATE;
}
<INITIAL>\\\\ BEGIN COMMENT;
<COMMENT>\n   BEGIN INITIAL;
<COMMENT>.                 ;
<INITIAL>[ \t]             ;
<INITIAL>. {
	fprintf(stderr, "lexical error.\n");
	exit(1);
}
<STRING_LITERAL_STATE>\" {
	yylval.identifier = close_string_literal();
	BEGIN INITIAL;
	return STRING_LITERAL;
}
<STRING_LITERAL_STATE>\n {
	add_string_literal('\n');
}
<STRING_LITERAL_STATE>\\\"      add_string_literal('"');
<STRING_LITERAL_STATE>\\n       add_string_literal('\n');
<STRING_LITERAL_STATE>\\t       add_string_literal('\t');
<STRING_LITERAL_STATE>\\\\      add_string_literal('\\');
<STRING_LITERAL_STATE>.         add_string_literal(yytext[0]);
%%
