#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stdlib.h>
#include <stdio.h>

typedef struct node node;
typedef struct list list;

/* struct representing an element of the linked list */
struct node{
	void* value; 
	node* next;
};

/* struct representing the linked list itself */
struct list{
	node* head;
	node* last;
	int length;
};

/* creates a linked list */
list* create_linked_list();

/* creates a node, given a pointer of a value */
node* create_node(void* value);

/* will create a node for the given value and add it to the linked list */
void add(list* list, void* value);

/* get node at given index */
node* get_node(list* list, int index);

/* remove a node at given index, also freeing it in the process */
void remove_node(list* list, int index, void (*free_function)(void*));

/* print a linked list, given a print function for the values of the nodes */
void print(list* , void (*print_function)(void*));

/* free the list, given a free function for the values of the nodes */
void free_list(list* list, void (*free_function)(void*), unsigned int remove_values); 

/* insert a value into a linked list based on the compare function, the list may only contain 'max' amount of nodes.
   if a node needs to be removed because 'max' amount has been reached, it will also be freed */
void sorted_insert(list* list, void* value, int (*compare)(void*, void*), int max, void (*free_function)(void*));

#endif