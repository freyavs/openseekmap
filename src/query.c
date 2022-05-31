#include "query.h"

void handle_queries(int loc_given, double lon, double lat, list* database, int algorithm){

    /* make the search object */
    search* search = malloc(sizeof(*search));
    check_pointer(search, "Couldn't allocate search.\n");
    search->location_given = loc_given;

    /* get input queries */
    char input[512] = {0};
    printf("?\n");
   
    while (fgets(input, 512, stdin)) {
        strtok(input, "\n"); /* remove trailing newline */
        filter_query(input);
        search->matches = create_linked_list();

        /* split query into different subqueries */
        int words = count_words(input);
        if (words > 0) {
            int possible_queries = pow(2,(words - 1));

            /* go over all split queries and update the best matches */
            for (int i = 0; i < possible_queries; i++){
                char* query_split = get_split_query(input, i);
                update_matches(i, query_split, database, search, algorithm);
            }
        } 
        print(search->matches, print_match);
        free_list(search->matches, free, 1);
        search->matches = NULL;

        /* get new query */
        printf("?\n");
    }
   free(search);
}

void filter_query(char* query){
    int len = strlen(query);
    char c;
    int i,x = 0;
    for (i=x= 0; i < len; i++) {
        c = query[i];
        /* switch control chars and punctuation marks to spaces */
        if ((iscntrl(c) && c != '\n') || ispunct(c)){
            query[i] = ' ';
        }
        /* remove spaces if necessary */
        if (!isspace(query[i]) || (i > 0 && !isspace(query[i-1]))) {
            query[x++] = query[i];
        }
    }
    query[x] = '\0';

    /* remove trailing whitespace */
    if (x > 1 && isspace(query[x-1])) {
        query[x-1] = '\0';
    }
}

char* get_split_query(char* query, unsigned int pattern){
    int len = strlen(query);
    char* split = malloc(len+1);
    check_pointer(split, "Couldn't allocate a split query.\n");

    /* null terminate the created string */
    split[len] = '\0';

    int ws_counter = 0;

    /* start at the end of string as we will be checking the bit pattern of 'pattern':
       e.g. if pattern = 2 (0b10), 0 should be looking at the last whitespace, 1 at the second to last etc. */
    for (int i = len-1; i >= 0; i--){
        if (isspace(query[i]) && ws_counter < pattern) {
            if (GET_BIT(pattern, ws_counter)){
                split[i] = SEPARATOR[0]; //take first character of string separator
            }
            else {
                split[i] = ' ';
            }
            ws_counter++;
        }
        else {
            split[i] = query[i];
        }
    }
    return split;
}
