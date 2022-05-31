#include <stdio.h>
#include <math.h>
#include <time.h>
#include "database.h"
#include "util.h"
#include "strings.h"
#include "linked_list.h"
#include "query.h"

int main(int argc, char *argv[]) {
   double lon = 0;
   double lat = 0;
   int loc_given = 0;

   /* check if arguments are correct */
   if (argc < 2 || argc > 5) {
      printf("USAGE:\n");
      printf("openseekmap --[alg1|alg2] <database file> <longitude> <latitude>\n");
      printf("openseekmap --[alg1|alg2] <database file>\n");
      printf("openseekmap <database file> <longitude> <latitude>\n");
      printf("openseekmap <database file>\n");
      return 1;
   }

   int alg = 1; /* this will be the default optimal version */
   int alg_specified = 0;
   if (argc == 5 || argc == 3){
      alg_specified = 1;
      if (strcmp(argv[1], "--alg1") == 0){
         alg = 1; /* already default */ 
      }
      else if (strcmp(argv[1], "--alg2") == 0){
         alg = 2;
      }
      else {
         printf("ERROR: Please use valid algorithm flag.\n");
         return 1;
      }
   }

   /* check for longitude and latitude arguments */
   if (argc >= 4) {
      lon = atof(argv[2+alg_specified]);
      lat = atof(argv[3+alg_specified]);
      loc_given = 1;
      if (lon > 180 || lon < -180 || lat > 90 || lat <-90){
         printf("ERROR: Please enter valid longitude and latitude values.\n");
         return 1;
      }
   }

   /* read in the database */
   list* db = read_database(argv[1+alg_specified]);

   /* start processing the queries */
   handle_queries(loc_given, lon, lat, db, alg);

   /* free all used memory */
   free_list(db, free_entry, 1);
   return 0;
}


