#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordonered_list.h"

// Fonction pour créer un nouveau nœud
Node* create_node(const char* key, int score) {
    Node* newNode = (Node*)malloc(sizeof(Node)); // Allocation mémoire
    if (newNode == NULL) {
        printf("Erreur : allocation mémoire échouée.\n");
        exit(1);
    }
    strncpy(newNode->elem.key, key, sizeof(newNode->elem.key) - 1); // Copier la clé en toute sécurité
    newNode->elem.key[sizeof(newNode->elem.key) - 1] = '\0';        // Assurer la terminaison de la chaîne
    newNode->elem.score = score;   // Initialiser le score
    newNode->next = NULL;          // Initialiser le pointeur suivant à NULL
    return newNode;
}

// Fonction pour insérer un nœud dans une liste triée (par score décroissant)
Node* add_node(Node* head, const char* key, int score) {
    Node* newNode = create_node(key, score);

    // Cas où la liste est vide ou le nouveau nœud doit être en tête
    if (head == NULL || score > head->elem.score) {
        newNode->next = head;
        return newNode;
    }

    // Parcourir la liste pour trouver la position d'insertion
    Node* temp = head;
    while (temp->next != NULL && temp->next->elem.score >= score) {
        temp = temp->next;
    }

    // Insérer le nouveau nœud à la bonne position
    newNode->next = temp->next;
    temp->next = newNode;

    return head;
}

// Fonction pour afficher la liste
void print_list(Node* head) {
    FILE* fd = fopen("/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/best_key_c3.txt", "w");
    Node* temp = head;
    while (temp != NULL) {
        // printf("Key: %s, Score: %d\n", temp->elem.key, temp->elem.score);
        fprintf(fd, "Key: %s, Score: %d\n", temp->elem.key, temp->elem.score);
        temp = temp->next;
    }
    fclose(fd);
}

// Libérer la mémoire de la liste
void free_list(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

// Fonction pour chiffrer et déchiffrer un texte avec une clé
void xor_crypt_decrypt(const char *ciphertext, char *plaintext, const char *key) {
    int i;
    for (i = 0; ciphertext[i] != '\0'; i++) {
        plaintext[i] = ciphertext[i] ^ key[i % strlen(key)];
    }
    plaintext[i] = '\0';
}

unsigned char* read_binary_file(const char *filename, size_t *out_len) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open file");
        return NULL;
    }

    // Seek to end to determine file size
    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek failed");
        fclose(fp);
        return NULL;
    }

    long filesize = ftell(fp);
    if (filesize < 0) {
        perror("ftell failed");
        fclose(fp);
        return NULL;
    }

    rewind(fp);

    unsigned char *buffer = (unsigned char*)malloc(filesize);
    if (!buffer) {
        perror("malloc failed");
        fclose(fp);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, filesize, fp);
    fclose(fp);

    if (read_size < (size_t)filesize) {
        fprintf(stderr, "Warning: Expected %ld bytes but only read %zu.\n", filesize, read_size);
        // Optionally handle partial reads
    }

    *out_len = read_size;
    return buffer;
}

void xor_crypt_decrypt_binary(const unsigned char *ciphertext, size_t cipher_len,
                              unsigned char *plaintext,
                              const unsigned char *key, size_t key_len) {
    for (size_t i = 0; i < cipher_len; i++) {
        plaintext[i] = ciphertext[i] ^ key[i % key_len];
    }
    // Optionally null-terminate if treating as string
    plaintext[cipher_len] = '\0';
}