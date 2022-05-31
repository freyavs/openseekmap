#ifndef _UTIL_H
#define _UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "../assignment/utf8.h"

#define GET_BIT(x,position) ((x >> (position)) & 1) /* get bit of x, same as checking if bit is set */
#define SET_BIT(x,position) ((x) | (1<<(position))) /* set a bit of x */

#define max(x,y) ((x)>(y)?x:y)
#define min(x,y) ((x)<(y)?x:y)

#define R 6371 /* radius of the earth in km */
#define TO_RAD (3.1415926536 / 180)

/* check if allocation succeeded, else give an error message */
void check_pointer(void* ptr, char* error_message);

/* print function for a character array */
void print_chars(void* str);

/* count amount of words in a character array */
int count_words(char* str);

/* count bits in a number */
int count_bits(int n);

/* haversine formula */
double d_geo(double lat1, double lon1, double lat2, double lon2);

/* get minimum of 3 int values */
int get_min(int x, int y, int z);

#endif