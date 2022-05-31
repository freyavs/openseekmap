
#ifndef _QUERY_H
#define _QUERY_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "match.h"
#include "util.h"
#include "linked_list.h"

#define SEPARATOR ";" /* character for separating parts of a query */

/* handle input of the query or queries, then find and print matches */
void handle_queries(int loc_given, double lon, double lat, list* database, int algorithm);

/* remove control characters and reduce whitespaces of a query */
void filter_query(char* query);

/* get different parts of query based on a bit pattern */
char* get_split_query(char* query, unsigned int pattern);

#endif