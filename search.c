/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

Searchres*
findindocfile(Docfile *df, char *terms)
{
	Searchres *sr = nil, *cursr;
	Function *curfn;
	char *tmp = nil;

	if(df->funs == nil)
		return nil;
	for(curfn = df->funs; curfn != nil; curfn = curfn->next){
		tmp = strcasestr(curfn->name, terms);
		if(tmp != nil){
			if(sr == nil){
				sr = emallocz(sizeof(Searchres));
				sr->fn = curfn;
				cursr = sr;
			} else {
				cursr->next = emallocz(sizeof(Searchres));
				cursr = cursr->next;
				cursr->fn = curfn;
			}
			tmp = nil;
		}
	}
	return sr;
}

void
searchresprinter(int ofd, Searchres *sr, int tab)
{
	int lastfn;

	if(sr->next == nil)
		lastfn = 1;
	else
		lastfn = 0;
	functionprinter(ofd, sr->fn, tab, lastfn);
}

void
allsearchresprinter(int ofd, Searchres *sr, int tab)
{
	Searchres *cur;

	if(sr == nil)
		return;
	for(cur = sr; cur != nil; cur = cur->next)
		searchresprinter(ofd, cur, tab);
}

