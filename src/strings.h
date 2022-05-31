#ifndef _STRINGS_H
#define _STRINGS_H

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "util.h"
#include "../assignment/utf8.h"
#include "../assignment/accents.h"

#define ASCII_LENGTH 128 /* not the extended ASCII, which would be 256 */

/* implementation of the simple shift and algorithm for exact matches for ASCII*/
int shift_and_ascii(char* z, char* t);

/* shift and allowing matches with k substitutions for ASCII*/
int shift_and_with_substitution_ascii(char* z, char* t, int k);

/* shift and allowing matches with k deletions/insertions/substitutions for ASCII */ 
int shift_and_with_errors_ascii(char* z, char* t, int k);

/* shift and allowing matches with k deletions/insertions/substitutions for UTF8 */
int shift_and_with_errors_utf8(char* z, char* t, int k);

/* dynamically programmed edit distance using a matrix, allowing deletions, insertions, substitutions and transpositions */
int edit_distance(char* a, char* b);

/* dynamically programmed edit distance using only 3 rows of a matrix, allowing deletions, insertions, substitutions and transpositions.
   this is the optimalized version of edit_distance */
int edit_distance_opt(char* a, char* b);


#endif