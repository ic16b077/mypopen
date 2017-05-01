/**
 * @file mypopen.c
 * Betriebssysteme Mypopen Header File.
 * Beispiel 2
 *
 * @author Aleksandar Pavlovic, Johanna Hoffmann, Michael Käfer
 * @date 2017/04/30
 *
 * @version 1
 *
 * @todo Review
 *
 */

/*
 * --------------------------------------------------------------- defines --
 */
#ifndef MYPOPEN_H
#define MYPOPEN_H

/*
 * -------------------------------------------------------------- includes --
 */
#include <stdio.h>

/*
 * ------------------------------------------------------------- functions --
 */
extern FILE *mypopen(const char *command, const char *type);
extern int mypclose(FILE *stream);

#endif

/*
 * =================================================================== eof ==
 */
