/*
 * Author: <Your Name>
 * Linked list data structure
 */

#ifndef LLIST_H
#define LLIST_H

// Node for the link list
typedef struct Node_ Node;
struct Node_ {
    void* data;
    Node* next;
};

// Create a new node with data and next element (can be NULL)
Node* node_new( void* data, Node* next );

// Link list structure
typedef struct LList_ {
    Node* head;
} LList;

// Create an empty list (head shall be NULL)
LList* llist_new();

// Traverse the linked list and return its size
int llist_size( LList* lst );

void* llist_get( LList* lst, int idx );
// Add a new element at the end of the list
LList* llist_append( LList* lst, void* data );
// Add a new element at the beginning of the list
LList* llist_prepend( LList* lst, void* data );
// Add a new element at the @idx index
LList* llist_insert( LList* lst, int idx, void* data );

// Remove an element from the end of the list
LList* llist_remove_last( LList* lst );
// Remove an element from the beginning of the list
LList* llist_remove_first( LList* lst );
// Remove an element from an arbitrary position in the list
LList* llist_remove( LList* lst, int idx );
// Clears the list
LList* llist_clear( LList* lst );

#endif // LLIST_H

