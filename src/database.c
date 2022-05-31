#include "database.h"
#include "util.h"
#include "linked_list.h"
#include "../assignment/utf8.h"

list* read_database(char* filename){

    list* database = create_linked_list();
    check_pointer(database, "Couldn't allocate database.");

    FILE* fp = fopen(filename, "r");
    check_pointer(fp, "Couldn't open database file, please make sure the file exists.\n");

    char line[1024]; 

    /* run through every line of the file and save them in database "linked list" */
    while (fgets(line, sizeof line, fp) != NULL) {
        entry* new_entry = calloc(1,sizeof(entry));
        check_pointer(new_entry, "Couldn't allocate entry of database.\n");
        read_line(line, new_entry);
        add(database, new_entry);
    }
    fclose(fp);
    return database;
}

/* this method is only used in tests, as it didn't show significant decrease in execution time of the program*/
list** read_database_name_based(char* filename){
    int size = 64;
    unsigned int entry_length = 0;
    list** database = malloc(size*sizeof(list*));

    for (int i=0; i<size; i++){
        database[i] = create_linked_list();
        check_pointer(database, "Couldn't allocate database.");
    }

    FILE* fp = fopen(filename, "r");
    check_pointer(fp, "Couldn't open database file, please make sure the file exists.\n");

    char line[1024];

    /* run through every line of the file and save them in database "linked list" */
    while (fgets(line, sizeof line, fp) != NULL) {
        entry* new_entry = malloc(sizeof(entry));
        check_pointer(new_entry, "Couldn't allocate entry of database.\n");
        read_line(line, new_entry);
        entry_length = u8_strlen(new_entry->name);

        /* larger entries names are saved in the last element of the array */
        if (entry_length-1 >= size) {
            add(database[size-1], new_entry);
        }
        else {
            add(database[entry_length-1], new_entry);
        }
    }
    fclose(fp);
    return database;
}

void read_line(char* line, entry* entry){
    char* delimiter = "\t";
    entry->identifier = atol(strtok(line, delimiter));
    char* field = strtok(NULL, delimiter); 

    /* save the name */
    int len = strlen(field);
    entry->name = malloc(len+1);
    check_pointer(entry->name, "Couldn't allocate name field of entry.\n");
    strcpy(entry->name, field);
    entry->name[len] = '\0';

    entry->rang = atoi(strtok(NULL, delimiter));
    entry->lon = atof(strtok(NULL, delimiter));

    /* null check, because the generated data - used in tests - sometimes misses the "name" field, so a line only has 4 fields */
    field = strtok(NULL, delimiter);
    if (field != NULL) entry->lat = atof(field);

}

void print_entry(void* e){
    printf("Entry: '%s'\n", (((entry*)e)->name));
}

void free_entry(void* e){
    free(((entry*)e)->name);
    free(e);
}

