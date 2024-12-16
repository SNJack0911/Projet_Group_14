#ifndef ORDONERED_LIST_H
#define ORDONERED_LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définir les structures
typedef struct Element {
    char key[50]; // Une clé (chaîne de caractères)
    int score;    // Un score (entier)
} Element;

typedef struct Node {
    Element elem;         // L'élément stocké dans ce nœud
    struct Node* next;    // Pointeur vers le nœud suivant
} Node;

// Déclaration des fonctions
Node* create_node(const char* key, int score);
Node* add_node(Node* head, const char* key, int score);
void print_list(Node* head);
void free_list(Node* head);
void xor_crypt_decrypt(const char *ciphertext, char *plaintext, const char *key);
void xor_crypt_decrypt_binary(const unsigned char *ciphertext, size_t cipher_len,unsigned char *plaintext,const unsigned char *key, size_t key_len); 
unsigned char *read_binary_file(const char *file_path, size_t *file_len);

#endif // ORDONERED_LIST_H
