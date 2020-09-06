/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"
#include "args.h"

void
usage(char *argv0)
{
	dprintf(2, "usage: %s [-h] [-S [-t n]] ", argv0);
	dprintf(2, "[-s search term] [-f output] [files...]\n");
	dprintf(2, "    -s search term\n");
	dprintf(2, "        search for a function definition and ");
	dprintf(2, "print the documentation\n");
	dprintf(2, "    -f output file\n");
	dprintf(2, "        pipe the output to a file\n");
	dprintf(2, "    -h\n");
	dprintf(2, "        print this message\n");
	dprintf(2, "    -S\n");
	dprintf(2, "        prefer spaces over tabs\n");
	dprintf(2, "    -t n\n");
	dprintf(2, "        use n spaces for tabulation (with -S)\n");
	exit(-1);
}

void
version(void)
{
	dprintf(2, "cdoc %s\n", VERSION);
	dprintf(2, "Copyright (c) 2019 Matthew Veety.\n");
	dprintf(2, "See LICENSE for details\n");
	exit(0);
}

int
dosearch(Docfile *df, char *searchterm, int ofd)
{
	Searchres *sr;

	sr = findindocfile(df, searchterm);
	if(sr == nil){
		dprintf(2, "error: unable to find %s\n", searchterm);
		return -2;
	}
	allsearchresprinter(ofd, sr, 0);
	return 0;
}

int
main(int argc, char *argv[])
{
	int flags[7] = {0, 0, 0, 0, 0, 0, 0};
	char *opts = "fhsdtSv";
	int argn;
	int i;
	int ifd;
	int ofd;
	char *ofname;
	char *tstop;
	char *searchterm;
	Docfile *df;

	if(argc < 2)
		usage(argv[0]);
	if((argn = args(argc, argv, &flags[0], opts)) < 0)
		usage(argv[0]);
	if(flags[3])
		debugprint = 1;
	if(flags[6])
		version();
	DBG("debugging enabled");
	if(flags[1])
		usage(argv[0]);
	if(flags[0])
		ofname = argf(argc, argv, 'f', usage);
	else
		ofname = "stdout";
	if(flags[2])
		searchterm = argf(argc, argv, 's', usage);
	if(flags[4]){
		tstop = argf(argc, argv, 't', usage);
		tabstop = atoi(tstop);
	}
	if(flags[5])
		tabtype = TabSpaces;
	if((argn = lastarg(argc, argv, "fst")) >= (argc - 1))
		usage(argv[0]);
	for(i = argn + 1; i < argc; i++){
		if((ifd = open(argv[i], O_RDONLY)) < 0)
			fileerror(argv[i], "unable to open file");
		tokfile(ifd, argv[i]);
		close(ifd);
		DBG("processed: %s", argv[i]);
	}
	df = makedocfile(ofname);
	if(flags[2]){
		ofd = 1;
		if(flags[0])
			if((ofd = open(ofname, O_WRONLY)) < 0)
				fileerror(ofname, "unable to open output file");
		return dosearch(df, searchterm, ofd);
	} else {
		if(flags[0])
			emitdocfile(df, ofname, -1);
		else
			emitdocfile(df, nil, 1);
		return 0;
	}
	return -1;	// never gets here
}

