#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LIGNS 100       // Nombre maximum de lignes à lire par batch
#define MAX_LEN_LIGNS 40 // Longueur maximum d'une ligne
#define MAX_SIZE_WORDTAB 100
#define MAX_LONGUEUR_MOT 40

// Fonction pour decrypter un fichier par la méthode XOR
int xor_crypt_decrypt(const char *text, const char *destination, const char *key) {
    FILE *file = fopen(text, "r");
    if (file == NULL) {
        printf("Error : Can't open file '%s'.\n", text);
        return 1;
    }
    FILE *destination_file = fopen(destination, "w");
    if (destination_file == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier '%s'.\n", destination);
        fclose(file);
        return 2;
    }

    int index_key = 0;
    int len_key = strlen(key);
    char carac;
    while ((carac = fgetc(file)) != EOF) {
        carac = carac ^ key[index_key];
        fwrite(&carac, sizeof(char), 1, destination_file);
        index_key++;
        if (index_key >= len_key) {
            index_key = 0; // Réinitialiser index_key après qu'il ait dépassé la longueur de la clé
        }
    }

    // Ajout d'un caractère nul à la fin du fichier destination
    fwrite("\0", sizeof(char), 1, destination_file);

    fclose(file);
    fclose(destination_file);
    return 0;
}

// Fonction pour rechercher un mot dans un dictionnaire
int search_in_dictionary(const char *word, const char *dictionary) {
    FILE *file = fopen(dictionary, "r"); // Ouvre le fichier en mode lecture
    if (file == NULL) {
        printf("Error : impossible to open file '%s'.\n", dictionary);
        return 3;
    }

    char *ligns_tab[MAX_LIGNS] = {NULL}; // Tableau pour stocker les lignes
    int count = 0;
    int found = 0; // Indicateur si le mot est trouvé

    // Lecture des lignes par lots de MAX_LIGNES
    while (!found) {
        count = 0;

        while (count < MAX_LIGNS) {
            char lign[MAX_LEN_LIGNS];
            // Lire une ligne du fichier
            if (fgets(lign, sizeof(lign), file) == NULL) {
                break; // Fin du fichier
            }
            // Supprimer le caractère de nouvelle ligne
            lign[strcspn(lign, "\n")] = '\0';

            // Allouer ou réallouer la mémoire pour stocker la ligne
            if (ligns_tab[count] == NULL) {
                ligns_tab[count] = malloc(strlen(lign) + 1);
            } else {
                ligns_tab[count] = realloc(ligns_tab[count], strlen(lign) + 1);
            }

            if (ligns_tab[count] == NULL) {
                printf("Error : insufficient memory.\n");
                fclose(file);
                return 4;
            }

            // Copier la ligne dans le tableau
            strcpy(ligns_tab[count], lign);
            // Comparer avec le mot
            if (strcmp(ligns_tab[count], word) == 0) {
                found = 1; // Mot trouvé
                break;
            }
            count++;
        }

        // Si on atteint la fin du fichier sans trouver le mot
        if (feof(file)) {
            break;
        }
    }

    fclose(file);
    // Libérer la mémoire allouée pour le tableau
    for (int i = 0; i < MAX_LIGNS; i++) {
        free(ligns_tab[i]);
    }

    return found; // Retourne 1 si le mot est trouvé, 0 sinon
}

// Fonction pour prélever les 100 premiers mots d'un texte et les placer dans un tableau
char ** text_to_tab(const char * text, int * word_count) {
    FILE * file = fopen(text,"r");
    if (file == NULL) {
        printf("Error : Can't open file '%s'.\n", text);
        return NULL;
    }

    char **word_tab = malloc(MAX_SIZE_WORDTAB * sizeof(char *)); // Tableau de pointeurs
     if (word_tab == NULL) {
         printf("Error : malloc failed.\n");
         fclose(file);
         return NULL;
     }
     char word[MAX_LONGUEUR_MOT];
     int index = 0;

    while (fscanf(file, "%39s", word) != EOF && index < MAX_SIZE_WORDTAB) { // Lire un mot
        // Convertir le mot en minuscules
        for (int i = 0; word[i] != '\0'; i++) {
            word[i] = tolower(word[i]);
        }
        word_tab[index] = malloc((strlen(word) + 1) * sizeof(char)); // Allouer mémoire pour chaque mot
        if (word_tab[index] == NULL) {
            printf("Error : malloc failed.\n");
            fclose(file);
            for (int i = 0; i < index; i++) { // Libérer mémoire allouée jusqu'à présent
                free(word_tab[i]);
            }
            free(word_tab);
            return NULL;
        }
        strcpy(word_tab[index], word); // Copier le mot dans le tableau
        index++;
    }

    fclose(file);
    *word_count = index;
    return word_tab;
}

// Fonction pour libérer la mémoire après text_to_tab
void free_word_tab(char **word_tab, int word_count) {
    for (int i = 0; i < word_count; i++) {
        free(word_tab[i]); // Libérer chaque mot
    }
    free(word_tab); // Libérer le tableau de pointeurs
}


// Fonction pour lire les clés depuis un fichier
char **read_keys(const char *file_path, int *keys_count) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier des clés '%s'.\n", file_path);
        return NULL;
    }

    // Compter les clés dans le fichier
    char line[256];
    *keys_count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        (*keys_count)++;
    }
    rewind(file);

    // Allouer un tableau pour les clés
    char **keys_tab = malloc(*keys_count * sizeof(char *));
    if (keys_tab == NULL) {
        printf("Erreur : Allocation mémoire pour les clés.\n");
        fclose(file);
        return NULL;
    }

    // Lire les clés dans le tableau
    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = '\0';  // Supprimer le '\n' à la fin de la ligne
        keys_tab[i] = strdup(line);       // Copier la clé
        if (keys_tab[i] == NULL) {
            printf("Erreur : Allocation mémoire pour une clé.\n");
            // Libérer la mémoire déjà allouée
            for (int j = 0; j < i; j++) {
                free(keys_tab[j]);
            }
            free(keys_tab);
            fclose(file);
            return NULL;
        }
        i++;
    }
    fclose(file);
    return keys_tab;
}
