#include "util.h"

void check_pointer(void* ptr, char* error_message){
    if (ptr == NULL){
        printf("ERROR: %s", error_message);
        exit(1);
    }
}

void print_chars(void* str){
    printf("List item: %s\n", (char*)str);
}

int count_words(char* str){
    int count = 0;
    int len = strlen(str);
    if (len == 0) return 0;
    for (int i = 0; i < len; i++){
        /* disregard a space at the end of the query */
        if (isspace(str[i]) && i < len-1) count++;
    }  
    /* n white spaces == (n+1) words */
    return ++count;
}

int count_bits(int n) 
{ 
    int count = 0; 
    while (n) { 
        count += n & 1; 
        n >>= 1; 
    } 
    return count; 
} 
 
double d_geo(double lat1, double lon1, double lat2, double lon2)
{
    double dx, dy, dz;
    lon1 = lon1 - lon2;
    lon1 *= TO_RAD, lat1 *= TO_RAD, lat2 *= TO_RAD;

    dz = sin(lat1) - sin(lat2);
    dx = cos(lon1) * cos(lat1) - cos(lat2);
    dy = sin(lon1) * cos(lat1);
    return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R; // km
}

int get_min(int x, int y, int z){
    return min(x, min(y, z));
}
