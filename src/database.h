#ifndef _DATABASE_H
#define _DATABASE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linked_list.h"

typedef struct entry entry;

/* struct representing a single database entry */
struct entry{
	uint64_t identifier; /* openstreetmap identifier */
	char* name; /* name of the place */
    int rang; /* rang - between 1 and 30 */
    double lon; /* longitude of the place (lengtegraad) */
    double lat; /* latitude of the place (breedtegraad) */
};

/* read the database into a linked list */
list* read_database(char* file);

/* read the database into an array of linked lists based on length of the name of an entry*/
list** read_database_name_based(char* file);

/* read a database line into an entry */
void read_line(char* line, entry* entry);

/* free an entry */
void free_entry(void* entry);

/* print an entry */
void print_entry(void* entry);

#endif