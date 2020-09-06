# cdoc
# Copyright (c) 2019 Matthew Veety.
# See LICENSE for details

include config.mk

NAME = cdoc
OBJECTS = main.o toks.o util.o args.o \
		  parse.o printer.o mem.o files.o \
		  search.o
LEXOBJS = lex.yy.o
SOURCES = $(OBJECTS:.o=.c)
HEADERS = args.h impl.h 

all: $(NAME)

allobjs: $(OBJECTS)

alldocs: cdoc.doc

alltests: test_lex test_toks test_makedocline
	./test_lex example.c
	./test_toks example.c
	./test_makedocline

cdoc.doc: $(NAME) $(SOURCES)
	./cdoc -f cdoc.doc $(SOURCES)

buildtests: test_lex test_toks test_makedocline

clean:
	rm -vf *.o test_*.o test_*.exe *.yy.c test_lex test_toks test_makedocline

distclean: clean
	rm -vf $(NAME) $(NAME).exe cdoc.doc

%.h: 

%.c:

%.l:

lex.yy.c: lex.l
	lex -o lex.yy.c lex.l

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c

$(NAME): $(OBJECTS) $(LEXOBJS)
	$(CC) -o $(NAME) $(OBJECTS) $(LEXOBJS)

test_lex: lex.yy.c
	$(CC) -o test_lex lex.yy.c -DDOTESTS

test_toks: lex.yy.o toks.o util.o test_toks.o
	$(CC) -o test_toks lex.yy.o toks.o util.o test_toks.o

test_makedocline: test_makedocline.o toks.o util.o parse.o mem.o
	$(CC) -o test_makedocline test_makedocline.o toks.o util.o parse.o mem.o


