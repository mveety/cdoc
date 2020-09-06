/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include "impl.h"

int
main(int argc, char *argv[])
{
	int fd;
	int i;

	if(argc < 2){
		dprintf(2, "usage: %s [test files]\n", argv[0]);
		exit(-1);
	}

	for(i = 1; i < argc; i++){
		if((fd = open(argv[i], O_RDONLY)) < 0){
			dprintf(2, "%s: panic: could no open file %s\n", argv[0], argv[i]);
			exit(-2);
		}
		tokfile(fd, argv[i]);
		close(fd);
	}
	fprintalltoks(1, "");
	return 0;
}

