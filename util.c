/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

volatile int debugprint = 0;

void
panic(char *s)
{
	dprintf(2, "cdoc: panic: %s\n", s);
	exit(-99);
}

void
error(char *s)
{
	if(toks != nil && curtok != nil){
		dprintf(2, "%s:%d: error: %s\n", curtok->fname, curtok->linenum, s);
		exit(-2);
	}
	dprintf(2, "error: %s\n", s);
	exit(-98);
}

void
fileerror(char *fname, char *s)
{
	dprintf(2, "%s: error: %s\n", fname, s);
	exit(-97);
}

void
warn(char *s)
{
	if(toks != nil && curtok != nil)
		dprintf(2, "%s:%d: warn: %s\n", curtok->fname, curtok->linenum, s);
	else
		dprintf(2, "warn: %s\n", s);
}

void*
emallocz(uintptr sz)
{
	void *ptr;

	if(!(ptr = malloc(sz)))
		panic("bad malloc");
	memset(ptr, 0, sz);
	return ptr;
}

