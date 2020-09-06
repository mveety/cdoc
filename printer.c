/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

int tabtype = TabNative;
int tabstop = 4;

void
_indentone(int ofd)
{
	int i;

	switch(tabtype){
	case TabNative:
		dprintf(ofd, "\t");
		break;
	case TabSpaces:
		for(i = 0; i < tabstop; i++)
			dprintf(ofd, " ");
		break;
	}
}

void
indent(int ofd, int depth)
{
	int i;

	if(depth == 0)
		return;
	for(i = 0; i < depth; i++)
		_indentone(ofd);
}

void
doclineprinter(int ofd, Docline *dl, int tab)
{
	indent(ofd, tab);
	dprintf(ofd, "%s\n", dl->dat);
}

void
functionprinter(int ofd, Function *fn, int tab, int lastfn)
{
	int i;
	Docline *cur;

	indent(ofd, tab);
	dprintf(ofd, "%s:%d", fn->fname, fn->linenum);
	indent(ofd, 1);
	dprintf(ofd, "%s %s(", fn->type, fn->name);
	if(fn->argn == 0){
		dprintf(ofd, "void");
	} else {
		for(i = 0; i < fn->argn; i++){
			if(i != 0)
				dprintf(ofd, ", ");
			dprintf(ofd, "%s %s", fn->argtypes[i], fn->argnames[i]);
		}
	}
	dprintf(ofd, ")\n");
	if(fn->doclines != nil)
		for(cur = fn->doclines; cur != nil; cur = cur->next)
			doclineprinter(ofd, cur, tab+1);
	if(!lastfn)
		dprintf(ofd, "\n");
}

void
docfileprinter(int ofd, Docfile *df, int tab, int printfname)
{
	Docline *dlcur;
	Function *fncur;

	if(printfname){
		indent(ofd, tab);
		dprintf(ofd, "%s\n\n", df->fname);
	}
	if(df->globaldocs != nil)
		for(dlcur = df->globaldocs; dlcur != nil; dlcur = dlcur->next)
			doclineprinter(ofd, dlcur, tab);
	dprintf(ofd, "\n");
	if(df->funs != nil)
		for(fncur = df->funs; fncur != nil; fncur = fncur->next)
			functionprinter(ofd, fncur, tab, fncur->next == nil ? 1 : 0);
	dprintf(ofd, "\n");
}

