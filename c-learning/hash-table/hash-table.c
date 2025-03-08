#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
    Naive hash table implementation based on CS50 concepts

    In essence, hash tables are arrays of linked lists, where each array 
      element represents a pointer to the first node in the linked list under that key.
      Since we're using arrays to hold the keys - re-sizing will be difficult. And since 
      we're using linked lists to hold nodes that fit under the same key, 
      operations involving these nodes will take longer - so we want to avoid key
      collision (we can control this with the hashing function chosen).

    Known issues with this implementation of a hash table:
    - table_size and table (the hash table itself) are implemented in global 
      variables. The creation of a hash table would be better off wrapped up
      in a function, which will also allow us to parameterise how big the initial 
      table is.
    - This would be more readable if we split off the function defns and structs 
      into separate files.
    - A function to delete the entire table hasn't been implemented yet.

*/


/*
    Initial params
*/

// Choosing a prime number for the initial size since that reduces collision chance even more
int table_size = 101;


/*
    Structs and data
*/

// Implementation of the linked list component
typedef struct node {

    char *key;
    char *value;
    struct node *next;

} node;

// Create array for keys
//   Creates an array (called table) of pointers to nodes of size table_size.
//   Then fills the array with NULL values to remove any leftover garbage values.
// NB: C doesn't allow variable length arrays in the global scope - workaround for now
node *table[101] = {NULL};


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

// Insert into the hash table
void insert(const char *key, const char *value) {

    // Get index in overarching array of hash table
    unsigned int index = hash(key, table_size);

    // Check if key already exists and update value
    node *current = table[index];
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
    new_node->key   = strdup(key);
    new_node->value = strdup(value);
    new_node->next  = table[index];  // Insert at the beginning of the linked list
    table[index]    = new_node;      // Update head pointer

}


// Get from hash table
char *get(const char *key) {

    // Get index in overarching array of hash table
    unsigned int index = hash(key, table_size);

    // Get pointer to first node under this key
    node *node_p = table[index];

    // Then from there, we just traverse the linked list until we find the key we're looking for
    while (node_p) {

        if (strcmp(node_p->key, key) == 0) {
            return node_p->value;
        }
        node_p = node_p->next;

    }

    // If we reach this point, we will not have found anything
    return NULL;

}

// Delete node
void delete(char *key) {

    // Get index in overarching array of hash table
    unsigned int index = hash(key, table_size);

    // Node pointer
    node *ptr = table[index];
    node *prev = NULL;

    if (!ptr) {
        printf("No nodes to delete\n");
        return;
    }

    // If node exists, traverse linked list to find where our node is
    while (ptr) {

        if (strcmp(ptr->key, key) == 0) {

            // If deleting the head node, update table[index]
            if (prev == NULL) {
                table[index] = ptr->next;
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
void print_table() {
    for (int i = 0; i < table_size; i++) {
        printf("[%d]: ", i);
        node *cursor = table[i];
        while (cursor) {
            printf("(%s, %s) -> ", cursor->key, cursor->value);
            cursor = cursor->next;
        }
        printf("NULL\n");
    }
}


int main(void) {

    insert("Charlie", "(634) 466-1630");
    insert("Mac", "1-436-705-3673");
    insert("Dee", "1-214-717-1808");
    insert("Dennis", "(491) 584-6065");

    char *name = "Agamemnon";
    char *result = get(name);  // Declare and initialize `result`
    if (result) {
        printf("Found %s: %s\n", name, result);
    } else {
        printf("%s not found\n", name);
    }

    name = "Dennis";
    result = get(name);  // result var already initialised
    if (result) {
        printf("Found %s: %s\n", name, result);
    } else {
        printf("%s not found\n", name);
    }

    print_table();

    delete("Agamemnon");
    delete("Dennis");

    print_table();

    return 0;

}

/*
    Result

    $ make hash-table
    $ ./hash-table
    > Agamemnon not found
    > Found Dennis: (491) 584-6065
    > [0]: NULL
    > [1]: NULL
    > [2]: NULL
    > [3]: NULL
    > [4]: NULL
    > [5]: NULL
    > [6]: NULL
    > [7]: NULL
    > [8]: NULL
    > [9]: NULL
    > [10]: NULL
    > [11]: NULL
    > [12]: NULL
    > [13]: NULL
    > [14]: NULL
    > [15]: NULL
    > [16]: NULL
    > [17]: NULL
    > [18]: NULL
    > [19]: NULL
    > [20]: NULL
    > [21]: NULL
    > [22]: NULL
    > [23]: NULL
    > [24]: NULL
    > [25]: NULL
    > [26]: NULL
    > [27]: NULL
    > [28]: NULL
    > [29]: NULL
    > [30]: (Dennis, (491) 584-6065) -> NULL
    > [31]: NULL
    > [32]: NULL
    > [33]: NULL
    > [34]: NULL
    > [35]: NULL
    > [36]: NULL
    > [37]: NULL
    > [38]: NULL
    > [39]: NULL
    > [40]: NULL
    > [41]: NULL
    > [42]: NULL
    > [43]: NULL
    > [44]: NULL
    > [45]: NULL
    > [46]: NULL
    > [47]: NULL
    > [48]: NULL
    > [49]: NULL
    > [50]: NULL
    > [51]: NULL
    > [52]: NULL
    > [53]: NULL
    > [54]: NULL
    > [55]: NULL
    > [56]: NULL
    > [57]: NULL
    > [58]: NULL
    > [59]: NULL
    > [60]: NULL
    > [61]: NULL
    > [62]: NULL
    > [63]: NULL
    > [64]: (Mac, 1-436-705-3673) -> NULL
    > [65]: NULL
    > [66]: NULL
    > [67]: NULL
    > [68]: (Charlie, (634) 466-1630) -> NULL
    > [69]: NULL
    > [70]: NULL
    > [71]: NULL
    > [72]: NULL
    > [73]: NULL
    > [74]: NULL
    > [75]: NULL
    > [76]: NULL
    > [77]: NULL
    > [78]: NULL
    > [79]: NULL
    > [80]: NULL
    > [81]: NULL
    > [82]: NULL
    > [83]: NULL
    > [84]: NULL
    > [85]: NULL
    > [86]: NULL
    > [87]: NULL
    > [88]: NULL
    > [89]: NULL
    > [90]: NULL
    > [91]: NULL
    > [92]: NULL
    > [93]: (Dee, 1-214-717-1808) -> NULL
    > [94]: NULL
    > [95]: NULL
    > [96]: NULL
    > [97]: NULL
    > [98]: NULL
    > [99]: NULL
    > [100]: NULL
    > No nodes to delete
    > Deleted key: Dennis
    > [0]: NULL
    > [1]: NULL
    > [2]: NULL
    > [3]: NULL
    > [4]: NULL
    > [5]: NULL
    > [6]: NULL
    > [7]: NULL
    > [8]: NULL
    > [9]: NULL
    > [10]: NULL
    > [11]: NULL
    > [12]: NULL
    > [13]: NULL
    > [14]: NULL
    > [15]: NULL
    > [16]: NULL
    > [17]: NULL
    > [18]: NULL
    > [19]: NULL
    > [20]: NULL
    > [21]: NULL
    > [22]: NULL
    > [23]: NULL
    > [24]: NULL
    > [25]: NULL
    > [26]: NULL
    > [27]: NULL
    > [28]: NULL
    > [29]: NULL
    > [30]: NULL
    > [31]: NULL
    > [32]: NULL
    > [33]: NULL
    > [34]: NULL
    > [35]: NULL
    > [36]: NULL
    > [37]: NULL
    > [38]: NULL
    > [39]: NULL
    > [40]: NULL
    > [41]: NULL
    > [42]: NULL
    > [43]: NULL
    > [44]: NULL
    > [45]: NULL
    > [46]: NULL
    > [47]: NULL
    > [48]: NULL
    > [49]: NULL
    > [50]: NULL
    > [51]: NULL
    > [52]: NULL
    > [53]: NULL
    > [54]: NULL
    > [55]: NULL
    > [56]: NULL
    > [57]: NULL
    > [58]: NULL
    > [59]: NULL
    > [60]: NULL
    > [61]: NULL
    > [62]: NULL
    > [63]: NULL
    > [64]: (Mac, 1-436-705-3673) -> NULL
    > [65]: NULL
    > [66]: NULL
    > [67]: NULL
    > [68]: (Charlie, (634) 466-1630) -> NULL
    > [69]: NULL
    > [70]: NULL
    > [71]: NULL
    > [72]: NULL
    > [73]: NULL
    > [74]: NULL
    > [75]: NULL
    > [76]: NULL
    > [77]: NULL
    > [78]: NULL
    > [79]: NULL
    > [80]: NULL
    > [81]: NULL
    > [82]: NULL
    > [83]: NULL
    > [84]: NULL
    > [85]: NULL
    > [86]: NULL
    > [87]: NULL
    > [88]: NULL
    > [89]: NULL
    > [90]: NULL
    > [91]: NULL
    > [92]: NULL
    > [93]: (Dee, 1-214-717-1808) -> NULL
    > [94]: NULL
    > [95]: NULL
    > [96]: NULL
    > [97]: NULL
    > [98]: NULL
    > [99]: NULL
    > [100]: NULL

*/