#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 100

/* Node for the linked list (handles hash collisions) */
typedef struct Node {
    char* word;
    int count;
    struct Node* next;
} Node;

/* Hash Table structure */
typedef struct {
    Node** buckets;
} HashTable;

/* Cross-platform string duplication (since strdup is POSIX, not standard C) */
char* my_strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = (char*)malloc(len);
    if (dup) {
        strcpy(dup, s);
    }
    return dup;
}

/* djb2 Hash Function by Dan Bernstein - excellent for strings */
unsigned int hash_function(const char* word) {
    unsigned long hash = 5381;
    int c;
    while ((c = *word++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % TABLE_SIZE;
}

/* Initialize the Hash Table */
HashTable* create_table() {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) return NULL;
    
    table->buckets = (Node**)calloc(TABLE_SIZE, sizeof(Node*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    return table;
}

/* Insert a word or increment its count if it already exists */
void insert_word(HashTable* table, const char* word) {
    unsigned int index = hash_function(word);
    Node* current = table->buckets[index];

    /* Check if word already exists in the chain */
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            current->count++;
            return;
        }
        current = current->next;
    }

    /* Word not found, create a new node and prepend it to the bucket */
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        fprintf(stderr, "Memory allocation failed for new node.\n");
        return;
    }
    
    new_node->word = my_strdup(word);
    new_node->count = 1;
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
}

/* Convert string to lowercase for case-insensitive counting */
void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

/* Clean up symbols from words (e.g., "hello!" becomes "hello") */
void clean_word(char* str) {
    char* src = str;
    char* dst = str;
    while (*src) {
        if (isalpha((unsigned char)*src)) {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
}

/* Display the contents and free memory simultaneously to prevent leaks */
void print_and_destroy_table(HashTable* table) {
    printf("\n--- Word Frequency Results ---\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = table->buckets[i];
        while (current != NULL) {
            printf("%-15s : %d\n", current->word, current->count);
            
            /* Free memory as we go */
            Node* temp = current;
            current = current->next;
            free(temp->word);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
    printf("------------------------------\n");
    printf("Memory successfully freed.\n");
}

int main() {
    printf("Starting Text Analysis...\n");
    
    HashTable* table = create_table();
    if (!table) {
        fprintf(stderr, "Failed to initialize hash table.\n");
        return 1;
    }

    /* Hardcoded sample text to ensure it runs out-of-the-box in Visual Studio */
    char sample_text[] = "C is a powerful general-purpose programming language. "
                         "It can be used to develop software like operating systems, "
                         "databases, compilers, and so on. C programming is an "
                         "excellent language to learn to program for beginners.";

    /* Tokenize the string using spaces and punctuation as delimiters */
    char* token = strtok(sample_text, " ,.-");
    
    while (token != NULL) {
        char buffer[100];
        strncpy(buffer, token, sizeof(buffer) - 1);
        buffer[sizeof(buffer) - 1] = '\0';
        
        clean_word(buffer);
        to_lowercase(buffer);
        
        if (strlen(buffer) > 0) {
            insert_word(table, buffer);
        }
        
        token = strtok(NULL, " ,.-");
    }

    print_and_destroy_table(table);

    return 0;
}