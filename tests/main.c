#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "../src/database.h"
#include "../src/util.h"
#include "../src/strings.h"
#include "../src/linked_list.h"
#include "../src/match.h"
#include "../src/query.h"
#include "../assignment/utf8.h"

#define ASSERT(condition) { if(!(condition)){ printf("TEST FAILED: %s at %s ( line %d )\n", #condition , __FILE__ ,  __LINE__ ); } }

void database_reads_everything(char* file){
    list* db = read_database(file);
    list** db_list = read_database_name_based(file);
    
    FILE* fp = fopen(file, "r");
    char ch;
    int lines = 0;
    while(!feof(fp)){
        ch = fgetc(fp);
        if(ch == '\n') lines++;
    }

    fclose(fp);

    int length_total = 0;
    for (int i = 0; i < 64; i++){
        length_total += db_list[i]->length;
        free_list(db_list[i], free_entry, 1);
    }

    /* amount of entries should equal amount of lines */
    ASSERT(lines == db->length);
    ASSERT(lines == length_total);

    free_list(db, free_entry, 1);
    free(db_list);
}

void shift_and_finds_correct_matches(){
    ASSERT(shift_and_ascii("ok", "stok"));
    ASSERT(!shift_and_ascii("lol","een appel"));
    ASSERT(shift_and_ascii("hallo", "hallo"));
    ASSERT(!shift_and_ascii("abcdefghijklmnop","g"));

   // tests with only substitutions 
   ASSERT(shift_and_with_substitution_ascii("abc", "xazckdmjf", 1));
   ASSERT(!shift_and_with_substitution_ascii("abc", "een aap", 1));
   ASSERT(shift_and_with_substitution_ascii("abc", "omg", 3));
   ASSERT(!shift_and_with_substitution_ascii("abc", "omg", 2));
   ASSERT(shift_and_with_substitution_ascii("abc", "omg", 5));
   ASSERT(shift_and_with_substitution_ascii("abc", "xyzaboabcdef", 0));

   ASSERT(!shift_and_with_substitution_ascii("abc", "", 3));
   ASSERT(!shift_and_with_substitution_ascii("abc", "fack", 1));

   //tests for substitutions, insertions and deletions
   ASSERT(shift_and_with_errors_ascii("abcc", "fack", 2)); 
   ASSERT(shift_and_with_errors_ascii("abcd", "osfadmf", 2));
   ASSERT(!shift_and_with_errors_ascii("abc", "ombxc", 1));
   ASSERT(shift_and_with_errors_ascii("abc", "bac", 1));

   // tests for the utf version of the above
   ASSERT(shift_and_with_errors_utf8("abcc", "fack", 2)); 
   ASSERT(shift_and_with_errors_utf8("abcd", "osfadmf", 2));
   ASSERT(shift_and_with_errors_utf8("faf", "cdfob", 2));
   ASSERT(!shift_and_with_errors_utf8("faf", "cdfob", 1));
   ASSERT(!shift_and_with_errors_utf8("abcB", "ombxc", 1));
   ASSERT(shift_and_with_errors_utf8("abc", "bac", 1));

   ASSERT(shift_and_with_errors_utf8("äbc", "abc", 0));
   ASSERT(shift_and_with_errors_utf8("Äbc", "abc", 0));
   ASSERT(shift_and_with_errors_utf8("Äbc", "oooabc", 0));
   ASSERT(shift_and_with_errors_utf8("vrïjhëïdśtráát bruhè", "vrijheidstraat bruhe", 0));
   ASSERT(shift_and_with_errors_utf8("ïdśtráát", "vrijheidstrat", 1));
   /* implementation should also work both ways */
   ASSERT(shift_and_with_errors_utf8("oooabcoo","Äbc", 0));
   ASSERT(shift_and_with_errors_utf8("Äbc", "abc", 0));
}


void linked_list_basic_tests(){
    list* list = create_linked_list();
    
    char* stra = malloc(5);
    strcpy(stra, "str0");
    char* strb = malloc(5);
    strcpy(strb, "str1");
    char* strc = malloc(5);
    strcpy(strc, "str2");

    add(list, stra);
    add(list, strb);
    add(list, strc);

    ASSERT(list->length == 3);
    ASSERT(!strcmp((char*)get_node(list, 0)->value,stra));
    ASSERT(!strcmp((char*)get_node(list, 1)->value,strb));
    ASSERT(!strcmp((char*)get_node(list, 2)->value,strc));
    
    remove_node(list,1, free);
    ASSERT(list->length == 2);
    ASSERT(!strcmp((char*)get_node(list,1)->value,strc));
    free_list(list, free,1);
}

void linked_list_inserted_sort_tests_with_match_structs(){
    list* list = create_linked_list();
    int matches = 7;
    match* m[matches];
    for (int i = 0; i<matches; i++) {
        m[i] = malloc(sizeof(match));
        m[i]->score = i;
    }

    sorted_insert(list, m[2], compare_matches, 5, free);
    sorted_insert(list, m[1], compare_matches, 5, free);
    sorted_insert(list, m[4], compare_matches, 5, free);

    ASSERT(((match*)get_node(list,0)->value)->score == 4);
    ASSERT(((match*)get_node(list,1)->value)->score == 2);
    ASSERT(((match*)get_node(list,2)->value)->score == 1);
    ASSERT(((match*)list->last->value)->score == 1);

    sorted_insert(list, m[5], compare_matches, 5, free);
    sorted_insert(list, m[3], compare_matches, 5, free);

    ASSERT(((match*)get_node(list,0)->value)->score == 5);
    ASSERT(((match*)get_node(list,1)->value)->score == 4);
    ASSERT(((match*)get_node(list,2)->value)->score == 3);
    ASSERT(((match*)get_node(list,3)->value)->score == 2);
    ASSERT(((match*)get_node(list,4)->value)->score == 1);
    ASSERT(((match*)list->last->value)->score == 1);

    sorted_insert(list, m[0], compare_matches, 5, free);
    sorted_insert(list, m[6], compare_matches, 5, free);

    ASSERT(((match*)get_node(list,0)->value)->score == 6);
    ASSERT(((match*)get_node(list,1)->value)->score == 5);
    ASSERT(((match*)get_node(list,2)->value)->score == 4);
    ASSERT(((match*)get_node(list,3)->value)->score == 3);
    ASSERT(((match*)get_node(list,4)->value)->score == 2);
    ASSERT(((match*)list->last->value)->score == 2);

    free_list(list, free,1);
}

void edit_distance_finds_correct_distance(){
    /* edit distance with transposition */
    ASSERT(edit_distance("lessen","feesten") == 3);
    ASSERT(edit_distance("abcdefg","daf") == 5);
    ASSERT(edit_distance("ab","abc") == 1);
    ASSERT(edit_distance("acb","abc") == 1);
    ASSERT(edit_distance("acB","abc") == 1);
    ASSERT(edit_distance("acéB","ace") == 1);
    ASSERT(edit_distance("Äbc", "abc") == 0);
    ASSERT(edit_distance("sentie °61", "sentier °61") == 1);
    ASSERT(edit_distance("vrïjhëïdśtráát bruhè", "vrijheidstraat bruhe") == 0);

    /* optimalized edit distance should have same results */
    ASSERT(edit_distance_opt("lessen","feesten") == 3);
    ASSERT(edit_distance_opt("abcdefg","daf") == 5);
    ASSERT(edit_distance_opt("ab","abc") == 1);
    ASSERT(edit_distance_opt("acb","abc") == 1);
    ASSERT(edit_distance_opt("acB","abc") == 1);
    ASSERT(edit_distance_opt("acéB","acebo") == 1);
    ASSERT(edit_distance_opt("Äbc", "abc") == 0);
    ASSERT(edit_distance_opt("sentie °61", "sentier °61") == 1);
    ASSERT(edit_distance("vrïjhëïdśtráát bruhè", "vrijheidstraat bruhe") == 0);
   }

void query_functions_test(){
    /* query filtering */
    char str[50] = "een     qu!ery $\n";
    filter_query(str);
    ASSERT(strcmp(str, "een qu ery") == 0);

    char strr[50] = "België ïś  éèñ !.lánd";
    filter_query(strr);
    ASSERT(strcmp(strr, "België ïś éèñ lánd") == 0);
}

void util_functions_test(){
    /* counting words */
    ASSERT(count_words("een lange zin.\n") == 3);
    ASSERT(count_words("een nog langere zin ") == 4);
    ASSERT(count_words("een") == 1);
    ASSERT(count_words("") == 0);

    /* counting bits */ 
    ASSERT(count_bits(0) == 0);
    ASSERT(count_bits(1) == 1);
    ASSERT(count_bits(3) == 2);
    ASSERT(count_bits(7845) == 8);
}

int main(int argc, char *argv[]) {

    printf("...Testing util functions.\n");
    util_functions_test();

    printf("...Testing database functions\n");
    database_reads_everything(argv[1]);

    printf("...Testing query functions\n");
    query_functions_test();

    printf("...Testing linked list functions\n");
    linked_list_basic_tests();

    printf("...Testing sorted insert\n");
    linked_list_inserted_sort_tests_with_match_structs();

    printf("...Testing string matching functions\n");
    shift_and_finds_correct_matches();
    edit_distance_finds_correct_distance();

    return 0;
}