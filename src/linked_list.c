#include "linked_list.h"
#include "util.h"


list* create_linked_list(){
    list* l = malloc(sizeof(list));
    check_pointer(l, "Memory allocation failed while creating linked list.");
    l->head = NULL;
    l->last = NULL;
    l->length = 0;
    return l;
}

node* create_node(void* value){
    node* n = malloc(sizeof(node));
    check_pointer(n, "Couldn't allocate node of linked list.");
    n->value = value;
    n->next = NULL;

    return n;
}

void free_node(node* node, void (*free_function)(void*)){
    (*free_function)(node->value);
    free(node);
}

void add(list* list, void* value){
    node* n = create_node(value);
    if (list->head == NULL){
        list->head = n;
        list->last = n;
    } else {
        list->last->next = n;
        list->last = n;
    }
    list->length++;
}

node* get_node(list* list, int index){
    node* it = list->head;
    int i = 0;
    if (index < 0) return NULL;
    else if (index == 0) return it;

    while (it->next != NULL){
        i++;
        it = it->next;
        if (index == i) return it;
    }
    /* index not found */
    return NULL;
}

void remove_node(list* list, int index, void (*free_function)(void*)){
    if (index < 0) return;
    int i = 0;
    node* prev;
    node* current = list->head;

    if (list->head == NULL) return;

    if (index == 0){
        list->head = list->head->next;
        free_node(current, free_function);
        list->length--;
        return;
    }

    while (current->next != NULL){
        i++;
        prev = current;
        current = current->next;
        if (index == i){
            list->length--;
            prev->next = current->next;
            if (prev->next == NULL) list->last = prev;
            free_node(current, free_function);
            return;
        }
    }
}

void free_list(list* list, void (*free_function)(void*), unsigned int remove_values){
    node* it = list->head;
    node* remove;

    if (it == NULL || it->value == NULL){
        free(list);
        return;
    }
    while (it->next != NULL){
        remove = it;
        it = it->next;
        if (remove_values) free_node(remove, free_function);
        else free(remove);
    }
    if (remove_values) free_node(it, free_function);
    else free(it);
    free(list);
}

void print(list* list, void (*print_function)(void*)){
    node* it = list->head;
    int i = 0;
    if (it == NULL || it->value == NULL){
        /* print for this application specifically */
        printf("No matches.\n");
        return;
    }
    while (it->next != NULL && i < 10){
        i++;
        (*print_function)(it->value);
        it = it->next;
    }
    (*print_function)(it->value);
}

void sorted_insert(list* list, void* value, int (*compare)(void*, void*), int max, void (*free_function)(void*)){
    node* it = list->head;
    node* prev = NULL;
    node* n;
    int inserted = 0;
    int i = 0;
    int done = 0;
    
    /* no nodes in the list yet */
    if (it == NULL){
        n = create_node(value);
        list->head = n;
        list->last = n;
        list->length++;
        return;
    }

    /* iterate over first 'max' elements and insert if possible */
    while (!done && i < max){
        if (!inserted && compare(value, it->value) > 0) {
            n = create_node(value);
            inserted = 1; /* set inserted to true */
            /* if value to be inserted is bigger than first element */
            if (prev == NULL){
                list->head = n;
                n->next = it; 
                prev = n;
            }
            else {
                prev->next = n;
                n->next = it;
                prev = n;
            }
        }
        if (it->next == NULL) done = 1;
        else {
            prev = it;
            it = it->next;
            i++;
        }
    }
    /* if something was inserted, remove last element */
    if (inserted && i >= max-1 ) {
        prev->next = NULL;
        list->last = prev; 
        free_function(it->value);
        free(it);
    }
    /* if nothing was inserted, and we havent reached max size of list yet, insert it */
    else if (!inserted && i < max-1){
        n = create_node(value);
        it->next = n;
        list->last = n;
        list->length++;
    }
    /* if the new value wasn't inserted, free it */
    else if (!inserted) {
        free_function(value);
    }
    
}


