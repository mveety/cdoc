/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

Token *toks;
Token *lasttok;
Token *curtok;
volatile int EOTpanic = 0; // panic if end of tokens

void
addtoken(char *str, int type, char *fname, int linenum)
{
	Token *tok;

	tok = emallocz(sizeof(Token));
	tok->type = type;
	tok->linenum = linenum;
	tok->fname = strdup(fname);
	tok->dat = strdup(str);
	tok->prev = nil;
	tok->next = nil;
	if(toks == nil){
		toks = tok;
		lasttok = tok;
		curtok = tok;
	} else {
		tok->prev = lasttok;
		lasttok->next = tok;
		lasttok = tok;
	}
}

Token*
getnexttoken(void)
{
	if(toks == nil || curtok == nil)
		panic("no tokens");
	if(curtok->next == nil){
		if(EOTpanic)
			error("premature end of tokens");
		else
			return nil;
	}
	curtok = curtok->next;
	return curtok;
}

Token*
getprevtoken(void)
{
	if(toks == nil || curtok == nil)
		panic("no tokens");
	if(curtok->prev == nil){
		if(EOTpanic)
			error("beginning of tokens");
		else
			return curtok;
	}
	curtok = curtok->prev;
	return curtok;
}

Token*
getcurtoken(void)
{
	return curtok;
}

Token*
getlasttoken(void)
{
	return lasttok;
}

void
freetoks(Token *tok)
{
	Token *cur;

	if(tok == nil)
		return;
	if(tok->next == nil){
		if(tok->prev != nil)
			tok->prev->next = nil;
		free(tok->fname);
		free(tok->dat);
		free(tok);
		return;
	}
	if(tok->prev != nil){	// cut the list at this point
		tok->prev->next = nil;
		tok->prev = nil;
	}
	for(cur = tok; cur->next != nil; cur = cur->next){
		if(cur->prev == nil)
			continue;
		free(cur->prev->fname);
		free(cur->prev->dat);
		free(cur->prev);
	}
	free(cur->fname);
	free(cur->dat);
	free(cur);
}

char*
toktypestring(int typ)
{
	switch(typ){
	case TokNone:
		return "TokNone";
	case TokDocline:
		return "TokDocline";
	case TokComment:
		return "TokComment";
	case TokPreproc:
		return "TokPreproc";
	case TokComstart:
		return "TokComstart";
	case TokComend:
		return "TokComend";
	case TokNumber:
		return "TokNumber";
	case TokString:
		return "TokString";
	case TokLCBracket:
		return "TokLCBracket";
	case TokRCBracket:
		return "TokRCBracket";
	case TokLParen:
		return "TokLParen";
	case TokRParen:
		return "TokRParen";
	case TokLSBracket:
		return "TokLSBracket";
	case TokRSBracket:
		return "TokRSBracket";
	case TokSemicolon:
		return "TokSemicolon";
	case TokSym:
		return "TokSym";
	case TokComma:
		return "TokComma";
	case TokAsterisk:
		return "TokAsterisk";
	case TokOther:
		return "TokOther";
	}
	return "(unknown)";
}

void
fprinttoken(int fd, char *prefix, Token *tok)
{
	dprintf(fd, "%s(Token %p){\n", prefix, tok);
	dprintf(fd, "%s\t.prev = %p\n", prefix, tok->prev);
	dprintf(fd, "%s\t.type = %s\n", prefix, toktypestring(tok->type));
	dprintf(fd, "%s\t.linenum = %d\n", prefix, tok->linenum);
	dprintf(fd, "%s\t.fname = \"%s\"\n", prefix, tok->fname);
	dprintf(fd, "%s\t.dat = \"%s\"\n", prefix, tok->dat);
	dprintf(fd, "%s\t.next = %p\n", prefix, tok->next);
	dprintf(fd, "%s};\n", prefix);
}

void
fprintalltoks(int fd, char *prefix)
{
	Token *cur;

	for(cur = toks; cur != nil; cur = cur->next)
		fprinttoken(fd, prefix, cur);
}

