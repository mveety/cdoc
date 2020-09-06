/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

#include <u.h>
#include <libc.h>

//@ this is some file wide docs
//@ more file wide docs
//@ even more file wide docs

void fnproto1(int x, int y);
void fnproto2(int, int);
void fnproto3(int a, int b);
int globalvar1;
int globalvar2;
extern int globalvar3;
int globalvar4 = 10;
volatile int globalvar5 = 0x05;

void
intswap(int *a, int *b)
{
//@ this function swaps a and b
// this is just a basic bitch comment

	int tmp;

	tmp = *a;
	*a = *b;	// this is another comment
	*b = tmp;
	dicks=123456;
	dicks=0x13f2a;
	dicks=012345;
}

uintptr*
thisissomefunction(void)
{
	return &dicks;
}

uintptr
sys$write(STREAM *s, char* *dat, uintptr* len)
{
	//@ this is some like vms shit or so
	return write(s->fd, dat, len);
}	

int
main(int argc, char *argv[])
{
//@ this is some function docs
//@ even more function docs
//@ yet more function docs
	/* this is a multi-line
	 * comment man
	 */

	printf("this is a test");
	/* this is another multiline comment */
	return 0;
}

int
fptrtest(uintptr (*noargs)(), int (*fna)(int, int), int (*fnb)(int, char, int))
{
	//@ this one tests how function pointers are handled
	printf("dicks\n");
	return 0;
}

int
arraytest(int array1[24], int array2[test], int array3[], int array4[4])
{
	//@ this function tests how arrays are handled
	return 0;
}

