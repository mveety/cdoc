/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

int
main(void)
{
	Token *t;
	Docline *dl;

	t = emallocz(sizeof(Token));
	t->type = TokDocline;
	t->linenum = 0;
	t->fname = "stdin";
	t->dat = "//@ this is a test line bitches";
	dl = makedocline(t);
	fprinttoken(1, "", t);
	printf("dl->dat = \"%s\"\n", dl->dat);
	printf("dl->fname = \"%s\"\n", dl->fname);
	printf("dl->linenum = %d\n", dl->linenum);
	return 0;
}

