#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
#include "break_head.h"

// Français
double french_letter_frequencies[ALPHABET_SIZE] = {
    14.715, 1.044, 3.183, 3.669, 17.194, 1.066, 0.866, 0.737, 7.529,
    0.613, 0.049, 5.456, 2.968, 7.095, 5.796, 2.521, 1.362, 6.553,
    7.948, 7.244, 6.311, 1.838, 0.049, 0.427, 0.128, 0.326
};

// Anglais
double english_letter_frequencies[ALPHABET_SIZE] = {
    8.167, 1.492, 2.782, 4.253, 12.702, 2.228, 2.015, 6.094, 6.966,
    0.153, 0.772, 4.025, 2.406, 6.749, 7.507, 1.929, 0.095, 5.987,
    6.327, 9.056, 2.758, 0.978, 2.360, 0.150, 1.974, 0.074
};

void exec_c2(const char *ciphertext_path, const char *language)  {
    printf("Début de C2...");
    const char *keys_file_path = "/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/best_key_c1.txt";
    const char *output_file_path = "/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/best_key_c2.txt";

    if (strcmp(language, "francais") != 0 && strcmp(language, "anglais") != 0) {
        printf("Problème de langue : seul français ou anglais disponible\n");
        exit(2);
    }

    // Lire le texte chiffré
    char *ciphertext = read_file(ciphertext_path);
    if (ciphertext == NULL) {
        fprintf(stderr, "Erreur lors de la lecture du fichier ciphertext.txt\n");
        exit(1);
    }

    // Lire les clés possibles depuis le fichier
    FILE *keys_file = fopen(keys_file_path, "r");
    if (!keys_file) {
        perror("Erreur d'ouverture du fichier de clés");
        exit(EXIT_FAILURE);
    }

    char line[100]; // Taille maximale d'une ligne
    int num_keys = 0;

    // Compter le nombre de clés
    while (fgets(line, sizeof(line), keys_file) != NULL) {
        num_keys++;
    }
    rewind(keys_file);

    // Allouer un tableau pour stocker les clés et leurs distances
    KeyDistance *key_distances = malloc(num_keys * sizeof(KeyDistance));
    if (key_distances == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le tableau de clés\n");
        fclose(keys_file);
        free(ciphertext);
        exit(1);
    }

    // Lire les clés et calculer les distances
    int i = 0;
    while (fgets(line, sizeof(line), keys_file) != NULL) {
        line[strcspn(line, "\n")] = '\0'; // Supprimer le saut de ligne
        key_distances[i].key = strdup(line);

        // Déchiffrer le texte avec la clé
        char *plaintext = malloc(strlen(ciphertext) + 1);
        decrypt_xor(ciphertext, plaintext, key_distances[i].key);

        // Calculer les fréquences des lettres
        double frequencies[ALPHABET_SIZE];
        calculate_frequencies(plaintext, frequencies);

        // Calculer la distance avec les fréquences théoriques
        double target_frequencies[ALPHABET_SIZE];
        if (strcmp(language, "francais") == 0) {
            memcpy(target_frequencies, french_letter_frequencies, sizeof(french_letter_frequencies));
        } else {
            memcpy(target_frequencies, english_letter_frequencies, sizeof(english_letter_frequencies));
        }

        key_distances[i].distance = calculate_distance(target_frequencies, frequencies);

        free(plaintext);
        i++;
    }
    fclose(keys_file);

    // Trier les clés par distance
    qsort(key_distances, num_keys, sizeof(KeyDistance), compare_key_distance);

    // Écrire les clés triées dans le fichier de sortie
    FILE *output_file = fopen(output_file_path, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }

    for (int i = 0; i < num_keys; i++) {
        fprintf(output_file, "%s\n", key_distances[i].key);
        free(key_distances[i].key);
    }

    fclose(output_file);
    free(key_distances);
    free(ciphertext);

    printf("Fin de C2\n");
    printf("Nombre de clefs = %d\n",num_keys);
}