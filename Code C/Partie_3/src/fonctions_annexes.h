#ifndef FONCTIONS_ANNEXES_H
#define FONCTIONS_ANNEXES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LIGNS 100       // Nombre maximum de lignes à lire par batch
#define MAX_LEN_LIGNS 40 // Longueur maximum d'une ligne
#define MAX_SIZE_WORDTAB 100
#define MAX_LONGUEUR_MOT 40

// Fonction pour decrypter un fichier par la méthode XOR
int xor_crypt_decrypt(const char *text, const char *destination, const char *key);

// Fonction pour rechercher un mot dans un dictionnaire
int search_in_dictionary(const char *word, const char *dictionary);

// Fonction pour prélever les 100 premiers mots d'un texte et les placer dans un tableau
char ** text_to_tab(const char * text, int * word_count);

// Fonction pour libérer la mémoire après text_to_tab
void free_word_tab(char **word_tab, int word_count);

char **read_keys(const char *file_path, int *keys_count);

#endif
