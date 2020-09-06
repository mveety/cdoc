/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <assert.h>
#include <setjmp.h>
#include <stddef.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>

// because, you know, i'm a plan 9 programmer

#define nil ((void*)0)
typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long uvlong;
typedef long long vlong;
typedef uvlong u64int;
typedef vlong s64int;
typedef uint8_t u8int;
typedef int8_t s8int;
typedef uint16_t u16int;
typedef int16_t s16int;
typedef uintptr_t uintptr;
typedef intptr_t intptr;
typedef uint u32int;
typedef int s32int;
typedef u32int uint32;
typedef s32int int32;
typedef u16int uint16;
typedef s16int int16;
typedef u64int uint64;
typedef s64int int64;
typedef u8int uint8;
typedef s8int int8;

// actual stuff

#define VERSION "release 1"
#define TOKMAX 256
#define MAXARGS 25
#define DBG1(fname, linen) dprintf(2, fname ":" #linen ": ")
#define DBG2(fn, ln) DBG1(fn, ln)
#define DBG(...) if(debugprint){ DBG2( __FILE__ , __LINE__ ); \
							dprintf(2, __VA_ARGS__); \
							dprintf(2, "\n"); }

typedef struct Docfile Docfile;
typedef struct Docline Docline;
typedef struct Function Function;
typedef struct Token Token;
typedef struct Searchres Searchres;

enum {
	TokNone,
	TokDocline,		// //@ ... \n
	TokComment,		// // ... \n (ignored)
	TokPreproc,		// # ... \n
	TokComstart,	// /* (ignored)
	TokComend,		// */ (ignored)
	TokNumber,		// [0-9][0-9a-zA-Z]*
	TokString,		// "..."
	TokLCBracket,	// {
	TokRCBracket,	// }
	TokLParen,		// (
	TokRParen,		// )
	TokLSBracket,	// [
	TokRSBracket,	// ]
	TokSemicolon,	// ;
	TokSym,			// symbols (vars, etc)
	TokComma,		// ,
	TokAsterisk,	// *
	TokEqual,		// =
	TokOther,		// everything else
};

enum {
	TabSpaces,
	TabNative,
};

struct Token {
	Token *prev;
	int type;
	int linenum;
	char *fname;
	char *dat;
	Token *next;
};

struct Function {
	char *fname;
	int linenum;
	char *type;
	char *name;
	int argn;
	char *argtypes[MAXARGS];
	char *argnames[MAXARGS];
	Docline *doclines;
	Function *next;
};

struct Docfile {
	char *fname;
	Docline *globaldocs;
	Function *funs;
	Docfile *next;
};

struct Docline {
	char *fname;
	int linenum;
	char *dat;
	Docline *next;
};

struct Searchres {
	Function *fn;
	Searchres *next;
};

// util.c
extern volatile int debugprint;
extern void panic(char *s);
extern void error(char *s);
extern void fileerror(char *fname, char *s);
extern void warn(char *s);
extern void *emallocz(uintptr sz);

// toks.c
extern Token *toks;
extern Token *curtok;
extern volatile int EOTpanic;
extern void addtoken(char *str, int type, char *fname, int linenum);
extern Token *getnexttoken(void);
extern Token *getprevtoken(void);
extern Token *getcurtoken(void);
extern Token *getlasttoken(void);
extern void freetoks(Token *tok);
extern char *toktypestring(int typ);
extern void fprinttoken(int fd, char *prefix, Token *tok);
extern void fprintalltoks(int fd, char *prefix);

// lex.l/lex.yy.c
extern void tokfile(int fd, char *fname);

// parse.c
extern Docline *makedocline(Token *tok);
extern Function *makefunction(void);
extern Docfile *makedocfile(char *fname);

// mem.c
extern void freedoclines(Docline *dl);
extern void freefun(Function *fn);
extern void freefuns(Function *fn);
extern void freedocfile(Docfile *df, int recurse);

// printer.c
extern int tabtype;
extern int tabstop;
extern void docfileprinter(int ofd, Docfile *df, int tab, int printfname);
extern void functionprinter(int ofd, Function *fn, int tab, int lastfn);

// files.c
extern void pipefds(int ifd, int ofd);
extern void pipefiles(char *ifile, char *ofile);
extern void readfile(char *ifile);
extern Docfile *processfile(char *ifile, int aifd);
extern void emitdocfile(Docfile *df, char *ofile, int aofd);
extern void cleanup(Docfile *df);

// search.c
extern Searchres *findindocfile(Docfile *df, char *terms);
extern void searchresprinter(int ofd, Searchres *sr, int tab);
extern void allsearchresprinter(int ofd, Searchres *sr, int tab);

// main.c

