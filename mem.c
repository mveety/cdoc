/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

void
freedoclines(Docline *dl)
{
	Docline *cur;
	Docline *prev;

	if(dl == nil)
		return;
	if(dl->next == nil){
		free(dl->fname);
		free(dl->dat);
		free(dl);
		return;
	}
	for(prev = dl, cur = dl->next; cur != nil; cur = cur->next){
		free(prev->fname);
		free(prev->dat);
		free(prev);
		prev = cur;
	}
	free(prev->fname);
	free(prev->dat);
	free(prev);
}

void
freefun(Function *fn)
{
	int i;

	free(fn->fname);
	free(fn->type);
	free(fn->name);
	for(i = 0; i < fn->argn; i++){
		free(fn->argtypes[i]);
		free(fn->argnames[i]);
	}
	freedoclines(fn->doclines);
	free(fn);
}

void
freefuns(Function *fn)
{
	Function *cur;
	Function *prev;

	if(fn == nil)
		return;
	if(fn->next == nil)
		freefun(fn);
	for(prev = fn, cur = fn->next; cur != nil; cur = cur->next){
		freefun(prev);
		prev = cur;
	}
	freefun(prev);
}

void
freedocfile(Docfile *df, int recurse)
{
	free(df->fname);
	freedoclines(df->globaldocs);
	freefuns(df->funs);
	if(recurse && df->next != nil)
		freedocfile(df->next, recurse);
	free(df);
}

