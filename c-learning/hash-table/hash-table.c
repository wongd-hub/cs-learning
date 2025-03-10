#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define TABLE_SIZE 11  // Define a fixed-size hash table

/*
    Naive hash table implementation based on CS50 concepts

    In essence, hash tables are arrays of linked lists, where each array 
      element represents a pointer to the first node in the linked list under that key.
      Since we're using arrays to hold the keys - re-sizing will be difficult. And since 
      we're using linked lists to hold nodes that fit under the same key, 
      operations involving these nodes will take longer - so we want to avoid key
      collision (we can control this with the hashing function chosen).

    Known issues with this implementation of a hash table:
    - This would be more readable if we split off the function defns and structs 
      into separate files.

*/


/*
    Structs and data
*/

// Implementation of the linked list component
typedef struct node {

    char *key;
    char *value;
    struct node *next;

} node;

// Overarching struct for the hash table
typedef struct {
    node *buckets[TABLE_SIZE]; // Array of pointers to linked lists
} hash_table;

/*
    Hash function

    We need a hash function that can adequately hash our keys, 
      whilst suppressing the probability of key collision. CS50 
      suggests a naive approach using the first letter of the name,
      but let's use DJB2 for now.

    djb2 hash function modified from http://www.cse.yorku.ca/~oz/hash.html

*/
unsigned int hash(const char *word, const int table_size) {

    unsigned long hash = 5381;
    int c;
    while ((c = *word++))
        hash = ((hash << 5) + hash) + c;
    return hash % table_size;  // Keep it within table size

}


/*
    Functions to interact with the hash table

*/

// Create hash table
hash_table *create_table() { // Returns a pointer to the hash table

    hash_table *ht = malloc(sizeof(hash_table));  // Allocate memory for the hash table struct

    if (!ht) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    // Initialize all buckets to NULL (drop all garbage values)
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->buckets[i] = NULL;
    }

    return ht;
}

// Insert into the hash table
void insert(hash_table *ht, const char *key, const char *value) {

    // Get index in overarching array of hash table
    unsigned int index = hash(key, TABLE_SIZE);

    // Check if key already exists and update value
    node *current = ht->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) { // If key exists, update value
            free(current->value);             // Yeet old value
            current->value = strdup(value);   // Update with new value
            return;                           // Exit without inserting a duplicate node
        }
        current = current->next;
    }

    // Allocate memory for the new node if doesn't already exist
    node *new_node = malloc(sizeof(node));

    if (!new_node) {
        printf("Memory allocation failed\n");
        return;
    }

    // Copy name and number
    new_node->key      = strdup(key);
    new_node->value    = strdup(value);
    new_node->next     = ht->buckets[index];  // Insert at the beginning of the linked list
    ht->buckets[index] = new_node;            // Update head pointer

}


// Get from hash table
char *get(hash_table *ht, const char *key) {

    // Get index in overarching array of hash table
    unsigned int index = hash(key, TABLE_SIZE);

    // Get pointer to first node under this key
    node *cursor = ht->buckets[index];

    // Then from there, we just traverse the linked list until we find the key we're looking for
    while (cursor) {

        if (strcmp(cursor->key, key) == 0) {
            return cursor->value;
        }
        cursor = cursor->next;

    }

    // If we reach this point, we will not have found anything
    return NULL;

}

// Delete node
void delete(hash_table *ht, const char *key) {

    // Get index in overarching array of hash table
    unsigned int index = hash(key, TABLE_SIZE);

    // Node pointer
    node *ptr  = ht->buckets[index];
    node *prev = NULL;

    if (!ptr) {
        printf("No nodes to delete\n");
        return;
    }

    // If node exists, traverse linked list to find where our node is
    while (ptr) {

        if (strcmp(ptr->key, key) == 0) {

            // If deleting the head node, update the hash table array
            if (prev == NULL) {
                ht->buckets[index] = ptr->next;
            } else {
                prev->next = ptr->next;   // Bypass the node being deleted
            }

            // Free memory
            free(ptr->key);
            free(ptr->value);
            free(ptr);
            printf("Deleted key: %s\n", key);
            return;  // Exit after deleting (assuming unique keys)

        }

        prev = ptr;      // Move prev to current node
        ptr = ptr->next; // Move to next node

    }

    printf("Key not found: %s\n", key);

}


// Print table
void print_table(hash_table *ht) {

    for (int i = 0; i < TABLE_SIZE; i++) {

        printf("[%d]: ", i);
        node *cursor = ht->buckets[i];
        while (cursor) {
            printf("(%s, %s) -> ", cursor->key, cursor->value);
            cursor = cursor->next;
        }
        printf("NULL\n");

    }

}

// Free table
void free_table(hash_table *ht) {

    for (int i = 0; i < TABLE_SIZE; i++) {

        node *cursor = ht->buckets[i];
        while (cursor) {
            node *temp = cursor;
            cursor = cursor->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }

    }

    free(ht);

}

int main(void) {

    hash_table *ht = create_table();

    insert(ht, "Charlie", "(634) 466-1630");
    insert(ht, "Mac", "1-436-705-3673");
    insert(ht, "Dee", "1-214-717-1808");
    insert(ht, "Dennis", "(491) 584-6065");
    insert(ht, "Frank", "(641) 848-9738");

    char *name = "Agamemnon";
    char *result = get(ht, name);  // Declare and initialize `result`
    if (result) {
        printf("Found %s: %s\n", name, result);
    } else {
        printf("%s not found\n", name);
    }

    name = "Dennis";
    result = get(ht, name);  // result var already initialised
    if (result) {
        printf("Found %s: %s\n", name, result);
    } else {
        printf("%s not found\n", name);
    }

    print_table(ht);

    delete(ht, "Agamemnon");
    delete(ht, "Dennis");

    print_table(ht);

    free_table(ht);

    return 0;

}

/*
    Result

    $ make hash-table
    $ ./hash-table
    > Agamemnon not found
    > Found Dennis: (491) 584-6065
    > [0]: (Mac, 1-436-705-3673) -> NULL
    > [1]: NULL
    > [2]: (Dee, 1-214-717-1808) -> (Charlie, (634) 466-1630) -> NULL
    > [3]: NULL
    > [4]: NULL
    > [5]: (Dennis, (491) 584-6065) -> NULL
    > [6]: NULL
    > [7]: NULL
    > [8]: (Frank, (641) 848-9738) -> NULL
    > [9]: NULL
    > [10]: NULL
    > Key not found: Agamemnon
    > Deleted key: Dennis
    > [0]: (Mac, 1-436-705-3673) -> NULL
    > [1]: NULL
    > [2]: (Dee, 1-214-717-1808) -> (Charlie, (634) 466-1630) -> NULL
    > [3]: NULL
    > [4]: NULL
    > [5]: NULL
    > [6]: NULL
    > [7]: NULL
    > [8]: (Frank, (641) 848-9738) -> NULL
    > [9]: NULL
    > [10]: NULL

*/