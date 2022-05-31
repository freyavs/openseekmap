#include "strings.h"

/* initial implementation of shift and for ASCII */
int shift_and_ascii(char* z, char* t){
    int m = strlen(z);
    int n = strlen(t);

    unsigned long int col = 0;
    int alphabet_len = ASCII_LENGTH;
    unsigned long int C[alphabet_len];

    /* initialize characteristic vectors for ASCII alphabet*/
    for (unsigned char k = 0; k < alphabet_len; k++) {
        C[k] = 0;
    }
    for (int i = 0; i < m; i++) {
        C[(unsigned char)z[i]] = SET_BIT(C[(unsigned char)z[i]],i);
    }

    /* start comparing */
    if (z[0] == t[0]) {
        if (strlen(z) == 1) return 1;
        col = SET_BIT(col,0); 
    }

    for (int j = 1; j < n; j++){
        col = SET_BIT((col << 1), 0);
        unsigned long int vector = C[(unsigned char)t[j]];
        col = col & vector;
        if (GET_BIT(col,m-1)) return 1;
    }

    return 0;
}

int shift_and_with_substitution_ascii(char* z, char* t, int k){
    int m = strlen(z);
    int n = strlen(t);

    unsigned long int cols[k+1];
    int alphabet_len = ASCII_LENGTH;
    unsigned long int C[alphabet_len];

    for (int i = 0; i <= k; i++ ) {
        cols[i] = 0;
    }
    /* initialize characteristic vectors for ASCII alphabet*/
    for (unsigned char k = 0; k < alphabet_len; k++) {
        C[k] = 0;
    }
    for (int i = 0; i < m; i++) {
        C[(unsigned char)z[i]] = SET_BIT(C[(unsigned char)z[i]],i);
    }

    /* start comparing */
    for (int j = 0; j < n; j++){
        
        unsigned long prev_col = cols[0];

        cols[0] = SET_BIT((cols[0] << 1), 0);
        unsigned long int vector = C[(unsigned char)t[j]];
        cols[0] = cols[0] & vector;

        for (int d=1; d<=k; d++){
            unsigned long current = cols[d];
            cols[d] = 1 | (prev_col << 1) | ( (cols[d] << 1) & (C[(unsigned char)t[j]]) );
            prev_col = current;

            if (GET_BIT(cols[d], m-1)) return 1; 
        }
        
        if (GET_BIT(cols[0], m-1)) return 1; 
    }

    return 0;
}


int shift_and_with_errors_ascii(char* z, char* t, int k){

    if (strlen(z) > strlen(t)) return shift_and_with_errors_ascii(t, z, k);

    int m = strlen(z);
    int n = strlen(t);

    unsigned long int cols[k+1];
    int alphabet_len = ASCII_LENGTH;
    unsigned long int C[alphabet_len];

    for (int i = 0; i <= k; i++ ) {
        cols[i] = 0;
    }
    /* initialize characteristic vectors for ASCII alphabet*/
    for (unsigned char k = 0; k < alphabet_len; k++) {
        C[k] = 0;
    }
    for (int i = 0; i < m; i++) {
        C[(unsigned char)z[i]] = SET_BIT(C[(unsigned char)z[i]],i);
    }

    /* start comparing */
    for (int j = 0; j < n; j++){
        unsigned long prev_col = cols[0];

        cols[0] = SET_BIT((cols[0] << 1), 0);
        unsigned long int vector = C[(unsigned char)t[j]];
        cols[0] = cols[0] & vector;

        for (int d=1; d<=k; d++){
            unsigned long current = cols[d]; 
            /* insertion, deletion, substitution or exact match */
            cols[d] =  prev_col | (cols[d-1] << 1) | (prev_col << 1) | 1 | ( (cols[d] << 1) & (C[(unsigned char)t[j]]) );
            prev_col = current;
        }

        if (GET_BIT(cols[k], m-1)) return 1;
    }

    return 0;
}

int shift_and_with_errors_utf8(char* z, char* t, int k){

    int m = u8_strlen(z);

    /* swap when length of searched string z is larger than t */
    if (m > u8_strlen(t)) return shift_and_with_errors_utf8(t, z, k);

    unsigned long int cols[k+1];
    /* length 128 because UTF8 1 byte codes start with 0, same as ASCII */
    int alphabet_len = ASCII_LENGTH;
    unsigned long int C[alphabet_len+m];

    for (int i = 0; i <= k; i++ ) {
        cols[i] = 0;
    }
    /* initialize characteristic vectors for ASCII alphabet, plus extra space for some UTF8 characters in the search string*/
    for (unsigned char k = 0; k < alphabet_len+m; k++) {
        C[k] = 0;
    }
    uint32_t c = 0;
    int index = 0;
    int i = 0;
    while((c = u8_nextchar(z,&index))){
        /* if c is one byte (ascii) character */
        if (c < alphabet_len){
            C[tolower(c)] = SET_BIT(C[tolower(c)],i);
        } else {
            /* try removing accent from c and check again if it's now a one byte (ascii) character */
            c = remove_accent(c);
            if (c < alphabet_len){
                C[tolower(c)] = SET_BIT(C[tolower(c)],i);
            /* c is not a one byte character */
            } else {
                int pos = 0;
                u8_strchr(z, c, &pos);
                /* add utf8 char behind the ascii chars, based on position of the first occurence in the search string */
                C[alphabet_len+pos-1] = SET_BIT(C[alphabet_len+pos-1],i);
            }
        }
        i++;
    }

    /* start comparing */
    index = 0;
    while((c = u8_nextchar(t,&index))){
        unsigned long prev_col = cols[0];
        cols[0] = SET_BIT((cols[0] << 1), 0);

        unsigned long int vector;
        /* use the same logic as above */
        if (c < alphabet_len){
            vector = C[tolower(c)];
        } else {
            c = remove_accent(c);
            if (c < alphabet_len){
                vector = C[tolower(c)];
            } else {
                int pos = 0;
                u8_strchr(z, c, &pos);
                vector = C[alphabet_len+pos-1];
            }
        }
        cols[0] = cols[0] & vector;

        for (int d=1; d<=k; d++){
            unsigned long current = cols[d]; 
            /*        insertion |     deletion     |      substitution      &      match     */
            cols[d] =  prev_col | (cols[d-1] << 1) | (prev_col << 1) | 1 | ( (cols[d] << 1) & vector);
            prev_col = current; //prev_col is dus de vector voor index j-1 met d-1 mismatches
        }

        if (GET_BIT(cols[k], m-1)) return 1;
    }

    return 0;
}

int edit_distance(char* a, char* b){
    int n = u8_strlen(a);
    int m = u8_strlen(b);

    int d[n+1][m+1];

    for (int i=0; i<n; i++){
        for (int j=0; j<m; j++){
            d[i][j]=0;
        }
    }
    int change_cost;
    
    for (int i=0; i<=n; i++) d[i][0] = i;
    for (int j=0; j<=m; j++) d[0][j] = j;

    int i = 1;
    int j;
    int index_a = 0;
    int index_b = 0;
    uint32_t ai;
    uint32_t prev_ai = '\0'; // to insure no warnings will be thrown
    uint32_t bi;
    uint32_t prev_bi = '\0';

    while ((ai = u8_nextchar(a,&index_a))){
        j = 1;
        index_b = 0;
        ai = tolower(remove_accent(ai));
        while ((bi = u8_nextchar(b,&index_b))){
            bi = tolower(remove_accent(bi));
            if (ai == bi) change_cost = 0;
            else change_cost = 1;
            /*              substitution            deletion      insertion */
            d[i][j] = get_min(d[i-1][j-1]+change_cost,  d[i-1][j]+1 ,d[i][j-1]+1);
            if (i>2 && j>2 && ai == prev_bi && prev_ai == bi) {
                d[i][j] = min(d[i][j], d[i-2][j-2] + 1); /* transposition */
            } 
            j++;
            prev_bi = bi;
        }
        i++;
        prev_ai = ai;
    }
    return d[n][m];
}

int edit_distance_opt(char* a, char* b){
    int m = u8_strlen(b);
    int change_cost;

    /* save 3 rows of the matrix, in stead of the whole matrix */
    int d[3][m+1]; 

    for (int i=0; i<=m; i++){
        d[0][i]=0;
    }
    for (int i=0; i<=m; i++) d[1][i] = i; //d[0][j]=j

    int i = 0;
    int j = 0;
    int index_a = 0;
    int index_b = 0;
    uint32_t ai;
    uint32_t prev_ai = '\0'; // to insure no warnings will be thrown
    uint32_t bi;
    uint32_t prev_bi = '\0';

    while ((ai = u8_nextchar(a,&index_a))){
        ai = tolower(remove_accent(ai));
        d[2][0] = i+1; //d[i][0] = i;
        j = 0;
        index_b = 0;
        while ((bi = u8_nextchar(b,&index_b))){
            bi = tolower(remove_accent(bi));
            if (ai == bi) change_cost = 0;
            else change_cost = 1;
            /*              substitution            deletion      insertion */
            d[2][j+1] = get_min(d[1][j]+change_cost,  d[1][j+1]+1 ,d[2][j]+1);
            if (i>1 && j>1 && ai == prev_bi && prev_ai == bi) {
                d[2][j+1] = min(d[2][j+1], d[0][j-1] + 1); /* transposition */
            } 
            j++;
            prev_bi = bi;
        }
        /* move rows down, so d[0] gets removed and replaced by d[1], d[1] gets replaced by d[2] */
        for (int j=0; j<=m; j++){
            d[0][j] = d[1][j];
            d[1][j] = d[2][j];
        }
        i++;
        prev_ai = ai;
    }
    return d[1][m];
}
