/*
 * Author: Rajan Bhargava
 * Linked list data structure (Implementation)
 */

#include <stdio.h>
#include "LList.h"
#include <stdlib.h>

Node* node_new( int data, Node* next )
{
    Node* new = malloc( sizeof(Node) );
    new -> data = data;
    new -> next = next;

    return new;
}

LList* llist_new()
{
    LList* new_list = malloc( sizeof(LList) );
    new_list -> head = node_new( 0, NULL );
    return new_list;
}

// considering head is not a part of the list
int llist_size( LList* list )
{
    int i = 0;

    Node* next_ptr;
    next_ptr = list -> head;

    while( next_ptr -> next != NULL ) {
        next_ptr = next_ptr -> next;
        i++;
    }

    return i;
}

void llist_print( LList* list )
{
    Node* next_ptr;
    next_ptr = list -> head;

    while( next_ptr != NULL ) {
        if( next_ptr != list -> head )
            printf("%d ", next_ptr -> data);

        next_ptr = next_ptr -> next;
    }
    printf("\n");
}

int llist_get( LList* lst, int idx )
{
    Node* next_ptr;
    int i;
    next_ptr = lst -> head;
    
    // next_ptr points to ith element
    for( i=-1 ; ; i++ ) {
        if( i == idx )
            return next_ptr -> data;
        next_ptr = next_ptr -> next;
    }

    return 0;
}

LList* llist_append( LList* lst, int data )
{
    Node* next_ptr;
    next_ptr = lst -> head;

    while( next_ptr -> next != NULL )
        next_ptr = next_ptr -> next;

   next_ptr->next = node_new( data, NULL );
    return lst;
}

// considering head is not a part of the list
LList* llist_prepend( LList* lst, int data )
{
    Node* new = node_new( data, ( lst -> head ) -> next );
    ( lst -> head ) -> next = new;

    return lst;
}

LList* llist_insert( LList* lst, int idx, int data )
{   
    Node* traverse_ptr;
    Node* new;
    int i;

    traverse_ptr = lst -> head;

    for( i=0; i<idx; i++ )
        traverse_ptr = traverse_ptr -> next;
    // traverse_ptr now points to the element at 'idx-1' position
    
    new = node_new( data, traverse_ptr -> next );
    traverse_ptr -> next = new;

    return lst;
}

LList* llist_remove_last( LList* lst )
{
    Node* next_ptr;
    next_ptr = lst -> head;

    if( next_ptr -> next == NULL )
        return lst;

    while( ( next_ptr -> next ) -> next != NULL )
        next_ptr = next_ptr -> next;

    /* 
     * Right now, the element after next_ptr points to NULL
     * Therefore, we should make this one point to NULL
     *  and deallocate memory of the last node
     */

    free( next_ptr -> next );
    next_ptr -> next = NULL;

    return lst;
}

LList* llist_remove_first( LList* lst )
{
    Node* rem = lst -> head -> next; 
    lst -> head -> next = lst -> head -> next -> next;
    free( rem );
    return lst;
} 

LList* llist_remove( LList* lst, int idx )
{
    int i;
    Node* traverse_list;
    traverse_list = lst -> head;
    
    for( i=0; i<idx; i++ )
        traverse_list = traverse_list -> next;

    // now at 'index-1'th element
    
    traverse_list -> next = traverse_list -> next -> next;

    return lst;
}


