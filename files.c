/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

void
cleanup(Docfile *df)
{
	if(toks != nil){
		freetoks(toks);
		toks = nil;
	}
	freedocfile(df, 1);
}

void
readfile(char *ifile)
{
	int ifd;

	if((ifd = open(ifile, O_RDONLY)) < 0)
		fileerror(ifile, "unable to open file to read");
	tokfile(ifd, ifile);
	close(ifd);
}

Docfile*
processfile(char *ifile, int aifd)
{
	int ifd;
	Docfile *df;
	char *fname;

	fname = ifile == nil ? "stdin" : ifile;
	if(aifd < 0){
		if((ifd = open(ifile, O_RDONLY)) < 0)
			fileerror(ifile, "unable to open file to read");
	} else
		ifd = aifd;
	tokfile(ifd, fname);
	df = makedocfile(fname);
	if(aifd < 0)
		close(ifd);
	return df;
}

void
emitdocfile(Docfile *df, char *ofile, int aofd)
{
	int ofd;

	if(aofd < 0){
		if((ofd = open(ofile, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
			fileerror(ofile, "unable to open file to write");
	} else
		ofd = aofd;
	docfileprinter(ofd, df, 0, 0);
	if(aofd < 0)
		close(ofd);
}

void
pipefiles(char *ifile, char *ofile)
{
	int ifd, ofd;

	if((ifd = open(ifile, O_RDONLY)) < 0)
		fileerror(ifile, "unable to open file to read");
	if((ofd = open(ofile, O_WRONLY | O_CREAT | O_TRUNC)) < 0)
		fileerror(ofile, "unable to open file to write");
	pipefds(ifd, ofd);
	close(ifd);
	close(ofd);
}

void
pipefds(int ifd, int ofd)
{
	Docfile *df;

	df = processfile(nil, ifd);
	emitdocfile(df, nil, ofd);
	cleanup(df);
}

