#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "break_head.h"
#include "ordonered_list.h"


//-----------------------------------------------------------FONCTION POUR CRACK C1---------------------------------------------------------------------//
// Fonction pour vérifier si un caractère est admissible
int isAdmissibleChar(int c) {
  return isalnum(c) || isspace(c) || ispunct(c);
}
void print_list_clef() {
    FILE *file = fopen("Code_C/src/Partie_3/src/print_cle_file.txt", "r");   // Open the file in read mode
    if (file == NULL) {                   // Check if the file was opened successfully
        perror("Error opening file");     // Print the error message
        exit(EXIT_FAILURE);               // Exit the program with a failure status
    }

    char buffer[1024];             // Buffer to store each line
    // Read each line from the file and print it
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    fclose(file);                         // Close the file
}

// Fonction pour générer les caractères admissibles pour chaque position dans la clé
void clefAdmissible(char *chiffre, char **clef, int key_length) {
    char clefCharacters[MAX_CARACTERES] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int tailleChiffre = strlen(chiffre);

    for (int pos_cle = 0; pos_cle < key_length; pos_cle++) {
        int candidat_pos = 0;
        for (int test_case = 0; test_case < MAX_CARACTERES; test_case++) {
            int idx_message = pos_cle;
            int candidat_valid = 1;
            while (idx_message < tailleChiffre) {
                if (!isAdmissibleChar(clefCharacters[test_case] ^ chiffre[idx_message])) {
                    candidat_valid = 0;
                    break; // If the character is not valid, stop checking further
                }
                idx_message += key_length;
            }
            if (candidat_valid) {
                if (candidat_pos < 61) {  // Prevent buffer overflow
                    clef[pos_cle][candidat_pos] = clefCharacters[test_case];
                    candidat_pos++;
                } else {
                    fprintf(stderr, "Warning: Too many candidates for clef[%d]. Truncating.\n", pos_cle);
                    break;
                }
            }
        }
        clef[pos_cle][candidat_pos] = '\0'; // Null-terminate the string
    }
}


// Fonction pour générer toutes les combinaisons cartésiennes des ensembles admissibles
void cartesianProductRecursive(char **sets, int *sizes, char *result, int index, int key_length,FILE*file) {
    if (index == key_length) {
        fwrite(result,sizeof(char),key_length,file);
        fputs("\n",file);
        return;
    }

    for (int i = 0; i < sizes[index]; i++) {
        result[index] = sets[index][i];
        cartesianProductRecursive(sets, sizes, result, index + 1, key_length,file);
    }
}
//-----------------------------------------------------------FONCTION POUR CRACK C2---------------------------------------------------------------------//
// Fréquences des lettres pour plusieurs langues (%)

// Fonction pour lire un fichier
char *read_file(const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Failed to open file '%s'\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (filesize < 0) {
        fprintf(stderr, "Error: Invalid file size for '%s'\n", filename);
        fclose(fp);
        return NULL;
    }

    char *content = (char*)malloc(filesize + 1);
    if (!content) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        fclose(fp);
        return NULL;
    }

    size_t read_size = fread(content, 1, filesize, fp);
    if (read_size != (size_t)filesize) {
        fprintf(stderr, "Error: Failed to read file '%s' completely.\n", filename);
        free(content);
        fclose(fp);
        return NULL;
    }
    content[filesize] = '\0';
    fclose(fp);

    return content;
}


// Fonction pour calculer les fréquences des lettres
void calculate_frequencies(const char *text, double frequencies[ALPHABET_SIZE]) {
    int letter_counts[ALPHABET_SIZE] = {0};
    int total_letters = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            letter_counts[text[i] - 'a']++;
            total_letters++;
        } else if (text[i] >= 'A' && text[i] <= 'Z') {
            letter_counts[text[i] - 'A']++;
            total_letters++;
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        frequencies[i] = total_letters > 0 ? (double)letter_counts[i] / total_letters * 100.0 : 0.0;
    }
}

// Fonction pour déchiffrer un texte avec une clé XOR
void decrypt_xor(const char *ciphertext, char *plaintext, const char *key) {
    int i;
    for (i = 0; ciphertext[i] != '\0'; i++) {
        plaintext[i] = ciphertext[i] ^ key[i % strlen(key)];
    }
    plaintext[i] = '\0';
}

// Fonction pour calculer la distance entre deux tableaux de fréquences
double calculate_distance(const double freq1[ALPHABET_SIZE], const double freq2[ALPHABET_SIZE]) {
    double distance = 0.0;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        distance += pow(freq1[i] - freq2[i], 2);
    }
    return sqrt(distance);
}

int compare_key_distance(const void *a, const void *b) {
    KeyDistance *kd1 = (KeyDistance *)a;
    KeyDistance *kd2 = (KeyDistance *)b;
    return (kd1->distance > kd2->distance) - (kd1->distance < kd2->distance);
}
//-----------------------------------------------------------FONCTION POUR CRACK C3---------------------------------------------------------------------//

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
// Function to convert a string to lowercase in-place
void to_lowercase(char* str) {
    if (str == NULL) return;
    for (; *str; ++str) {
        *str = tolower((unsigned char)*str);
    }
}

// Fonction pour prélever les 100 premiers mots d'un texte et les placer dans un tableau
char ** text_to_tab(const char * text, int * word_count) {
    FILE * file = fopen(text, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Can't open file '%s'.\n", text);
        return NULL;
    }

    // Allocate memory for the array of word pointers
    char **word_tab = malloc(MAX_SIZE_WORDTAB * sizeof(char *));
    if (word_tab == NULL) {
        fprintf(stderr, "Error: malloc failed for word_tab.\n");
        fclose(file);
        return NULL;
    }

    char line[MAX_LEN_LIGNS]; // Buffer to store each line read from the file
    int index = 0;              // Current word index in word_tab

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL && index < MAX_SIZE_WORDTAB) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        // Tokenize the line using space and comma as delimiters
        char *token = strtok(line, " ,\t\r"); // Added tab and carriage return as delimiters
        while (token != NULL && index < MAX_SIZE_WORDTAB) {
            // Convert the token to lowercase
            to_lowercase(token);

            // Allocate memory for the word and copy it
            word_tab[index] = malloc(strlen(token) + 1);
            if (word_tab[index] == NULL) {
                fprintf(stderr, "Error: malloc failed for word_tab[%d].\n", index);
                // Free previously allocated words
                for (int j = 0; j < index; j++) {
                    free(word_tab[j]);
                }
                free(word_tab);
                fclose(file);
                return NULL;
            }
            strcpy(word_tab[index], token);
            index++;

            // Get the next token
            token = strtok(NULL, " ,\t\r");
        }
    }

    fclose(file);
    *word_count = index;
    return word_tab;
}

// Lis un fichier de clés (une clé par ligne)
char **read_keys(const char *file_path,int *keys_count) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier des clés '%s'.\n", file_path);
        return NULL;
    }

    // Compter les lignes (clés)
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        count++;
    }
    rewind(file);

    // Allouer un tableau de pointeurs de chaînes
    char **keys_tab = malloc(count * sizeof(char *));
    if (keys_tab == NULL) {
        printf("Erreur : Allocation mémoire pour les clés.\n");
        fclose(file);
        return NULL;
    }

    // Lire chaque clé
    int i = 0;
    while ((i<count)&&(fgets(line, sizeof(line), file)) != NULL) {
        line[strcspn(line, "\n")] = '\0';  // enlever le '\n'
        keys_tab[i] = strdup(line);
        if (keys_tab[i] == NULL) {
            printf("Erreur : Allocation mémoire pour une clé.\n");
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
    *keys_count = count;
    return keys_tab;
}

// Fonction principale pour le déchiffrement (C3)
char* break_code_c3(const char **keys_tab, int keys_count,const char *dictionary, const char *ciphertext_file_path)
{
    // Lire le ciphertext en binaire
    size_t cipher_len = 0;
    unsigned char *cipher_data = read_binary_file(ciphertext_file_path, &cipher_len);
    if (!cipher_data) {
        fprintf(stderr, "Error: Failed to read ciphertext file '%s'.\n", ciphertext_file_path);
        return NULL;
    }

    Node* head = NULL;  // linked list for (key,score)

    for (int i = 0; i < keys_count; i++) {
        int word_count = 0;
        int score = 0;
        size_t key_len = strlen(keys_tab[i]);

        // Allouer un buffer plaintext
        unsigned char *plaintext = malloc(cipher_len + 1);

        // XOR déchiffrement
        xor_crypt_decrypt_binary(cipher_data, cipher_len,plaintext, (const unsigned char *)keys_tab[i],key_len);

        // Écrire le plaintext dans un fichier ASCII
        FILE *outfp = fopen("/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/decrypt_text.txt", "w");
        if (outfp) {
            fwrite(plaintext, 1, cipher_len, outfp);
            fclose(outfp);
        }
        free(plaintext);

        // Lire les mots du fichier déchiffré
        char **word_tab = text_to_tab("/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/decrypt_text.txt", &word_count);
        if (word_tab) {
            // Score via dictionnaire
            for (int j = 0; j < word_count; j++) {
                score += search_in_dictionary(word_tab[j], dictionary);
            }

            // Libération
            for (int j = 0; j < word_count; j++) {
                free(word_tab[j]);
            }
            free(word_tab);
        }

        // Ajouter (clé, score) à la liste
        head = add_node(head, keys_tab[i], score);
    }

    // Affiche la liste triée (du meilleur score au pire, selon add_node / sorting logic)
    char *best_key = strdup(head->elem.key);
    int best_score = head ->elem.score;
    printf("Meilleur score = %d\n",best_score);
    print_list(head);
    free_list(head);

    free(cipher_data);  // On libère le ciphertext binaire après usage
    return best_key;
}