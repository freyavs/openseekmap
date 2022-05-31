#include <time.h>
#include "match.h"

double EXTEND_MAPPING[] = {    
  -1,
  100000,
  50000,
  10000,
  1000,
  750,
  600,
  500,
  350,
  200,
  100,
  75,
  50,
  10,
  9,
  7.5,
  6,
  4.5,
  4,
  2,
  1,
  0.8,
  0.7,
  0.6,
  0.5,
  0.4,
  0.3,
  0.2,
  0.1,
  0.05,
  0.01
};

int compare_matches(void* m1, void* m2){
    double result = ((match*)m1)->score - ((match*)m2)->score;
    if (result < 0) return -1;
    else return ceil(result);
}

void print_match(void* m){
    entry* entry = ((match*)m)->entry;
    printf("\n");
    printf("%s (%ld)\n", entry->name, entry->identifier);
    printf("geo:%f,%f\n", entry->lat, entry->lon); 
    printf("~\n");
}

void update_matches(int index, char* query, list* db, search* search, int algorithm){
    node* entry_node = db->head;
    char* delimiter = SEPARATOR;
    unsigned int matches_found = 0;

    /* index bit pattern will show us where separations are, add 1 to see how many parts */
    int parts = count_bits(index) + 1;
    /* keep track of M1..Mn */
    list* part_matches[parts];
    char* queries[parts];

    /* keep parts of the query in an array */
    char* field = strtok(query, delimiter);
    int j = 0;
    while (field != NULL){
            queries[j] = field;
            field = strtok(NULL, delimiter);
            j++;
        }

    /* initialise array of linked lists for M1 .. Mn with n=parts*/
    for (int i = 0; i < parts; i++){
        part_matches[i] = create_linked_list();
    }

    /* go through database */
    while (entry_node){
        entry* e = (entry*)entry_node->value;
        int max_d;
        int len;

        /* check for all query fields if there is a match */
        for (int i=0; i < parts; i++) {
            len = u8_strlen(queries[i]);
            max_d = min(3, 1 + (len/3));
            /* check if lengths allow for less than 3 insertions/deletions to find a match, then with shift and */
            if (abs(len-u8_strlen(e->name)) < 3){
                if (shift_and_with_errors_utf8(queries[i], e->name, max_d*2)){
                    if (edit_distance_opt(queries[i], e->name) <= max_d) {
                        add(part_matches[i], e);
                        matches_found = SET_BIT(matches_found, i);
                    }
                } 
            }
        }

        entry_node = entry_node->next;
    }  
    /* we collected all matching entries for all parts of the query, now go through all possible total matches to find best matches */
    /* only go through matches if for every query part there was at least 1 match */
    if (matches_found == pow(2, parts)-1) {
        entry* total_match[parts];
        /* depending on which algorithm is chosen, use different combining methods */
        if (algorithm == 1) parse_matches_recursive_opt(queries, part_matches, parts, 0, total_match, search, 0, 0, 0, NULL);
        else parse_matches_recursive(queries, part_matches, parts, 0, total_match, search);
    }

    /* free memory used */
    for (int i = 0; i < parts; i++){
        free_list(part_matches[i], free, 0);
    }
    free(query);
}

void parse_matches_recursive(char* part_queries[], list* part_matches[], int parts, int index, entry* total_match[], search* search){

    /* if we got a total match, calculate its score */ 
    if (index == parts){
        match* m = calculate_match(part_queries, total_match, parts, search);
        /* check if a match was created to be added, if match is NULL then it should not be added */
        if (m) sorted_insert(search->matches, m, compare_matches, 5, free);
        return;
    }
    node* entry_node = part_matches[index]->head;
    while (entry_node) {
        entry* e = (entry*)(entry_node->value);
        total_match[index] = e;
        parse_matches_recursive(part_queries, part_matches, parts, index+1, total_match, search);
        entry_node = entry_node->next;
    }
}

/* faster but less clean implementation of the above */
void parse_matches_recursive_opt(char* part_queries[], list* part_matches[], int parts, int index, entry* total_match[], search* search, double lengths_z, double diffs, double synergy, entry* max_rang){
    /* if we reached a total match, calculate the final score and (if needed) add to matches */
    if (index == parts){
        double lowest_score = -1;
        if (search->matches->last){
            lowest_score = ((match*)(search->matches->last->value))->score;
        }
        double score = get_total_score(lengths_z, diffs, synergy, max_rang, search);
        if (score > lowest_score){
            match* m = malloc(sizeof(match));
            check_pointer(m, "Couldn't allocate match while making final match.\n");
            m->entry = max_rang;
            m->score = score;
            sorted_insert(search->matches, m, compare_matches, 5, free);
        }

        return;
    }
    node* entry_node = part_matches[index]->head;
    while (entry_node) {
        double temp_synergy = synergy;
        double temp_lengths_z = lengths_z;
        double temp_diffs = diffs;
        entry* max = max_rang;
        entry* e = (entry*)(entry_node->value);
        /* calculate correctness parts */
        temp_lengths_z += (double)u8_strlen(part_queries[index]);
        temp_diffs += (double)u8_strlen(part_queries[index]) - (double)edit_distance(part_queries[index], e->name);
        /* calculate synergy */
        for (int j = 0; j<index; j++){
            double s = synergy_part(total_match[j],e);
            temp_synergy += (s / (((double)parts)*(((double)parts)-1.0f)));
        }
        /* calculate highest rang */
        if (index == 0 || e->rang > max_rang->rang) {
            max = e;
        }
        total_match[index] = e;
        parse_matches_recursive_opt(part_queries, part_matches, parts, index+1, total_match, search, temp_lengths_z, temp_diffs, temp_synergy, max);
        entry_node = entry_node->next;
    }
}

match* calculate_match(char* z[], entry* v[], int n, search* search){
    double synergy = 0;
    entry* max_rang_entry = v[0];
    double lengths_z = 0;
    double diffs = 0;
    double score = 0;

    double lowest_score = -1;
    if (search->matches->last){
        score = ((match*)(search->matches->last->value))->score;
    }

    for (int i = 0; i<n;i++){

        /* calculate parts correctness */
        lengths_z += (double)u8_strlen(z[i]);
        diffs += (double)u8_strlen(z[i]) - (double)edit_distance(z[i], v[i]->name);

        /* calculate synergy */
        for (int j = i+1; j<n; j++){
            double s = synergy_part(v[i],v[j]);
            synergy += (s / (((double)n)*(((double)n)-1.0f)));
        }
        /* calculate highest rang */
        if (v[i]->rang > max_rang_entry->rang) {
            max_rang_entry = v[i];
        }
    }

    score = get_total_score(lengths_z, diffs, synergy, max_rang_entry, search);

    if (score > lowest_score){
        match* m = malloc(sizeof(match));
        check_pointer(m, "Couldn't allocate match while making final match.\n");
        m->entry = max_rang_entry;
        m->score = score;
        return m;
    }
    return NULL;
}

double synergy_part(entry* vi, entry* vj){
    double s =  
            max(EXTEND_MAPPING[vi->rang],EXTEND_MAPPING[vj->rang])/d_geo(vi->lat, vi->lon, vj->lat, vj->lon);
    return min(2,s*s);
}

double get_total_score(double lengths_z, double diffs, double synergy, entry* max_rang_entry, search* search){
    double correctness = diffs/lengths_z;

    if (search->location_given) {
        /* calculate score based on location given to our program */
        double log_result = log(d_geo(search->lat, search->lon, max_rang_entry->lat, max_rang_entry->lon)/EXTEND_MAPPING[max_rang_entry->rang]);
        double loc_score = min(1, max(0, 1-(log_result/log(1000))));
        return 0.15*correctness + 0.55*synergy + 0.15* (max_rang_entry->rang/30.0f) + 0.15*loc_score;
    }
    return (0.2*correctness) + (0.7*synergy) + (0.1*max_rang_entry->rang/30.0f);

}