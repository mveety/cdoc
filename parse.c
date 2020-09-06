/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

Docline*
makedocline(Token *tok)
{
	int len;
	char *rstr;
	Docline *dl;

	if(tok->type != TokDocline)
		return nil;
	dl = emallocz(sizeof(Docline));
	len = strlen(tok->dat);
	rstr = strdup(tok->dat);
	dl->dat = strdup(&rstr[4]);
	free(rstr);
	dl->fname = tok->fname;
	dl->linenum = tok->linenum;
	dl->next = nil;
	return dl;
}

// match for this: int[ ][*]*dicks\(
int
isfuncstart(void)
{
	Token *tok;
	Token *ct;
	volatile int st = 0;

	ct = curtok;
	tok = getcurtoken();
	if(!tok || tok->type != TokSym)
		goto done;
	for(tok = getnexttoken();
			tok != nil;
			tok = getnexttoken())
		if(tok->type != TokAsterisk)
			break;
	if(!tok || tok->type != TokSym)
		goto done;
	tok = getnexttoken();
	if(!tok || tok->type != TokLParen)
		goto done;
	st = 1;
done:
	curtok = ct;
	return st;
}

char*
parseargname(int *noadvance)
{
	char *buf;
	char *rval;
	int i = 0;
	int plevel = 0;
	Token *tok;

	buf = emallocz(TOKMAX);
	tok = getcurtoken();
	for(;;){
		switch(tok->type){
		case TokComma:
			if(plevel <= 0)
				goto doneloop;
			buf[i++] = ',';
			break;
		case TokRParen:
			if(plevel <= 0){
				*noadvance = 1;
				goto doneloop;
			}
			plevel--;
			buf[i++] = ')';
			break;
		case TokLParen:
			plevel++;
			buf[i++] = '(';
			break;
		default:
			strcpy(&buf[i], tok->dat);
			i += strlen(tok->dat);
			break;
		}
		tok = getnexttoken();
	}
doneloop:
	rval = strdup(buf);
	free(buf);
	return rval;
}

void
parseargs(Function *fn)
{
	int argi = 0;
	int i = 0;
	Token *tok;
	char *buf;
	int noadvance = 0;

	buf = emallocz(TOKMAX);
	for(;;){
		if(noadvance)
			noadvance = 0;
		else
			tok = getnexttoken();
		DBG("starttok: \"%s\" (%s)", tok->dat, toktypestring(tok->type));
		if(tok->type == TokComma)
			continue;
		if(tok->type == TokRParen)
			break;
		if(tok->type == TokSemicolon){
			DBG("found unexpected semicolon. backing out");
			getprevtoken();
			goto done;
		}
		if(tok->type != TokSym)
			continue;
		if(strcmp(tok->dat, "void") == 0){
			tok = getnexttoken();
			if(tok->type == TokRParen)
				break;
			tok = getprevtoken();
		}
		strcpy(buf, tok->dat);
		i = strlen(tok->dat);
		for(tok = getnexttoken();
				tok->type == TokAsterisk;
				tok = getnexttoken()){
			buf[i] = '*';
			i++;
		}
		fn->argtypes[argi] = strdup(buf);
		if(tok->type == TokLParen)
			fn->argnames[argi] = parseargname(&noadvance);
		else{
			if(tok->next != nil && tok->next->type == TokLSBracket){
				DBG("var is array");
				memset(buf, 0, TOKMAX);
				i = 0;
				strcpy(buf, tok->dat);
				i += strlen(tok->dat);
				tok = getnexttoken();
				for(;;){
					if(tok->type == TokRSBracket)
						break;
					DBG("vartok: \"%s\" (%s)", tok->dat,
							toktypestring(tok->type));
					if(tok->type == TokLSBracket)
						buf[i++] = '[';
					else{
						strcpy(&buf[i], tok->dat);
						i += strlen(tok->dat);
					}
					tok = getnexttoken();
				}
				buf[i] = ']';
				i++;
				fn->argnames[argi] = strdup(buf);
			} else
				fn->argnames[argi] = strdup(tok->dat);
		}
		memset(buf, 0, TOKMAX);
		argi++;
		tok = getcurtoken();
		DBG("endtok: \"%s\" (%s)", tok->dat, toktypestring(tok->type));
		DBG("noadvance = %d", noadvance);
	}
	fn->argn = argi;
done:
	free(buf);
}

Function*
makefunction(void)
{
	Token *tok;
	Function *fn;
	int i = 0;
	char *buf;
	int infun = 0;
	Docline *dl;
	Docline *cur;

	if(isfuncstart() == 0)
		return nil;
	EOTpanic = 1;
	buf = emallocz(TOKMAX);
	fn = emallocz(sizeof(Function));
	tok = getcurtoken();
	DBG("found function at %s:%d \"%s\" (%s)",
			tok->fname, tok->linenum, tok->dat,
			toktypestring(tok->type));
	fn->fname = strdup(tok->fname);
	fn->linenum = tok->linenum;
	strcpy(&buf[0], tok->dat);
	i = strlen(tok->dat);
	for(tok = getnexttoken(); tok->type == TokAsterisk; tok = getnexttoken()){
		buf[i] = '*';
		i++;
	}
	fn->type = strdup(&buf[0]);
	memset(&buf[0], 0, TOKMAX);
	i = 0;
	fn->name = strdup(tok->dat);
	getnexttoken();
	DBG("tok after fn->name %s:%d \"%s\" (%s)", tok->fname, tok->linenum,
			tok->dat, toktypestring(tok->type));
	parseargs(fn);
	DBG("tok after parseargs %s:%d \"%s\" (%s)", tok->fname, tok->linenum,
		tok->dat, toktypestring(tok->type));
	tok = getnexttoken();
	if(tok->type == TokSemicolon){
		DBG("found function prototype. backing out");
		free(buf);
		freefun(fn);
		return nil;
	}
	if(tok->type == TokLCBracket)
		infun = 1;
	for(tok = getnexttoken(); infun != 0; tok = getnexttoken()){
		switch(tok->type){
		case TokRCBracket:
			infun--;
			if(infun == 0)
				goto leaveloop;	// bit of a hack here
			break;
		case TokLCBracket:
			infun++;
			break;
		case TokDocline:
			dl = makedocline(tok);
			if(fn->doclines == nil){
				fn->doclines = dl;
				cur = dl;
			} else {
				cur->next = dl;
				cur = dl;
			}
			break;
		}
	}
leaveloop:
	EOTpanic = 0;
	free(buf);
	return fn;
}

Docfile*
makedocfile(char *fname)
{
	Token *cur;
	Docfile *df;
	Docline *dl;
	Docline *curdl;
	Function *fn;
	Function *curfn;

	df = emallocz(sizeof(Docfile));
	df->fname = strdup(fname);
	for(cur = getcurtoken(); cur != nil; cur = getnexttoken()){
		if(cur->type == TokDocline){
			dl = makedocline(cur);
			if(df->globaldocs == nil){
				df->globaldocs = dl;
				curdl = dl;
			} else {
				curdl->next = dl;
				curdl = curdl->next;
			}
		} else if(cur->type == TokSym){
			if(isfuncstart()){
				fn = makefunction();
				if(fn == nil)
					continue;
				if(df->funs == nil){
					df->funs = fn;
					curfn = fn;
				} else {
					curfn->next = fn;
					curfn = curfn->next;
				}
			}
		}
	}
	return df;
}

