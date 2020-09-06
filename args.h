/*
 * cdoc
 * Copyright (c) 2019 Matthew Veety.
 * See LICENSE for details
 */

extern int args(int argc, char *argv[], int *flags, char *options);
extern char *argf(int argc, char *argv[], char c, void (*fn)(char*));
extern int lastarg(int argc, char *argv[], char *opts);

