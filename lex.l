%{
/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"
int incomment = 0;
int linen = 1;
int bracketlvl = 0;
char *filename;
#define ADDTOKEN(x) addtoken(yytext, x, filename, linen)
%}

%option noyywrap

%%

[ \t] ;

^\#.* {
	if(!incomment)
#ifdef DOTESTS
		printf("%s:%d: found preprocessor line \"%s\"\n", filename, linen, yytext);
#else
		ADDTOKEN(TokPreproc);
#endif
}

[a-zA-Z_$][a-zA-Z0-9_$@]* {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found TokSym: \"%s\"\n", filename, linen, yytext);
#else
		ADDTOKEN(TokSym);
#endif
}

[0-9][a-zA-Z0-9]* {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found number \"%s\"\n", filename, linen, yytext);
#else
	ADDTOKEN(TokNumber);
#endif
}

(\/\/\@)[ ].* {
#ifdef DOTESTS
	printf("%s:%d: found TokDocline: \"%s\"\n", filename, linen, yytext);
#else
	ADDTOKEN(TokDocline);
#endif
}

(\/\/)[^\@].*$ {
#ifdef DOTESTS
	printf("%s:%d: found comment\n", filename, linen);
#else
#endif
}

(\/\*) {
#ifdef DOTESTS
	printf("%s:%d: found multiline comment start\n", filename, linen);
#endif
	incomment = 1;
}

(\*\/) {
#ifdef DOTESTS
	printf("%s:%d: found multiline comment end\n", filename, linen);
#endif
	incomment = 0;
}

\{ {
	if(!incomment){
#ifdef DOTESTS
		printf("%s:%d: found lbracket\n", filename, linen);
#else
		ADDTOKEN(TokLCBracket);
#endif
		bracketlvl++;
	}
}

\} {
	if(!incomment){
#ifdef DOTESTS
		printf("%s:%d: found rbracket\n", filename, linen);
#else
		ADDTOKEN(TokRCBracket);
#endif
		bracketlvl--;
	}
}

\[ {
	if(!incomment){
#ifdef DOTESTS
		printf("%s:%d: found lsbracket\n", filename, linen);
#else
		ADDTOKEN(TokLSBracket);
#endif
	}
}

\] {
	if(!incomment){
#ifdef DOTESTS
		printf("%s:%d: found rsbracket\n", filename, linen);
#else
		ADDTOKEN(TokRSBracket);
#endif
	}
}
\( {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found lparens\n", filename, linen);
#else
		ADDTOKEN(TokLParen);
#endif
}

\) {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found rparens\n", filename, linen);
#else
		ADDTOKEN(TokRParen);
#endif

}

\".*\" {
		if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found string:\"%s\"\n", filename, linen, yytext);
#else
		ADDTOKEN(TokString);
#endif
}

\; {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found semicolon\n", filename, linen);
#else
		ADDTOKEN(TokSemicolon);
#endif
}

\, {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found comma\n", filename, linen);
#else
		ADDTOKEN(TokComma);
#endif
}

\* {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found asterisk\n", filename, linen);
#else
		ADDTOKEN(TokAsterisk);
#endif
}

\= {
	if(!incomment && bracketlvl == 0)
#ifdef DOTESTS
		printf("%s:%d: found equal sign\n", filename, linen);
#else
		ADDTOKEN(TokEqual);
#endif
}

\n { linen++; }

. ;

%%

#ifdef DOTESTS

int
main(int argc, char *argv[])
{
	FILE *file;

	if(argc < 2)
		filename = "stdin";
	else {
		file = fopen(argv[1], "r");
		if(!file){
			printf("error: could not open %s\n", argv[1]);
			return -1;
		}
		yyin = file;
		filename = argv[1];
	}
	yylex();
	return 0;
}

#else

void
tokfile(int fd, char *fname)
{
	FILE *file;

	incomment = 0;
	linen = 1;
	bracketlvl = 0;
	if(fd != 0){
		if(!(file = fdopen(fd, "r")))
			panic("could not convert fd to FILE");
		yyin = file;
		filename = fname;
	} else {
		yyin = stdin;
		filename = "stdin";
	}
	yylex();
	// fdclose seems to be a BSD-ism. It's not strictly needed so I'll just
	// remove it until I can work out a similar replacement.
	// fdclose(file, nil);
}

#endif

