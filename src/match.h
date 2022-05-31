#ifndef _MATCH_H
#define _MATCH_H

#include "linked_list.h"
#include "util.h"
#include "database.h"
#include "strings.h"
#include "query.h"

/* struct representing a match, with the highest ranking entry and the total score of the match */
typedef struct match {
    entry* entry;
    double score;
} match;

/* struct representing a search, with the location given to the program and the list of current matches */
typedef struct search {
    double lon;
    double lat;
    int location_given;
    list* matches;
} search;

/* print a match */
void print_match(void* match);

/* compare two matches based on their score */
int compare_matches(void* m1, void* m2);

/* calculate synergy for two entries */
double synergy_part(entry* vi, entry* vj);

/* get the total score of a match, based on some calculations */
double get_total_score(double lengths_z, double diffs, double synergy, entry* max_rang_entry, search* search);

/* go over database and update the matches for a given query and an index representing the bit pattern we used to split the query */
void update_matches(int index, char* query, list* db, search* search, int algorithm);

/* combine all matches and calculate their scores recursively */
void parse_matches_recursive(char* part_queries[], list* part_matches[], int parts, int index, entry* total_match[], search* search);

/* combine all matches and calculate their scores recursively, the optimalized/faster version by calculating the score of a match while combining them at the same time*/
/* faster but maybe less comprehensive as it takes a lot of parameters */
void parse_matches_recursive_opt(char* part_queries[], list* part_matches[], int parts, int index, entry* total_match[], search* search, double lengths_z, double diffs, double synergy, entry* max_rang);

/* calculate the score of a match and return either a match if it should be added to the matches, or NULL if it shouldn't */
match* calculate_match(char* z[], entry* v[], int n, search* search);


#endif

