#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ALPHABET_SIZE 26

// Fréquences des lettres pour plusieurs langues (%)

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

// Fonction pour lire un fichier
char *read_file(const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Erreur d'ouverture de fichier");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);
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

int main(int argc, char *argv []) {
    const char *ciphertext_path = "cyphertext.txt";
    const char *keys_file_path = "listclefsadmissible.txt"; // Fichier généré par C1
    const char *output_file_path = "best_keys.txt"; 
    if (argc!=2){
        printf("Usage <langue>\n");
        exit(1);
    }

    const char *language = argv[1];
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
    char *key_str;
    int num_keys = 0;

    // Compter le nombre de clés
    while (fgets(line, sizeof(line), keys_file) != NULL) {
        num_keys++;
    }
    rewind(keys_file);

    // Allouer un tableau pour stocker les clés
    char **keys = (char **)malloc(num_keys * sizeof(char *));
    if (keys == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour le tableau de clés\n");
        fclose(keys_file);
        free(ciphertext);
        exit(1);
    }

    for (int i = 0; i < num_keys; i++) {
        keys[i] = (char *)malloc(100); 
        if (keys[i] == NULL) {
            fprintf(stderr, "Erreur d'allocation mémoire pour une clé\n");
            // Libérer la mémoire déjà allouée
            for (int j = 0; j < i; j++) {
                free(keys[j]);
            }
            free(keys);
            fclose(keys_file);
            free(ciphertext);
            exit(1);
        }
    }

    // Lire les clés du fichier
    int i = 0;
    while (fgets(line, sizeof(line), keys_file) != NULL) {
        key_str = strtok(line, "\n"); 
        strcpy(keys[i], key_str);
        i++;
    }
    fclose(keys_file);

    // Variables pour la meilleure clé
    int best_key_index = -1;
    double best_distance = INFINITY;

    // Fichier pour écrire les résultats
    FILE *output_file = fopen(output_file_path, "w");
    if (output_file == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie\n");
        exit(1);
    }

    // Tester chaque clé
    for (int k = 0; k < num_keys; k++) {
        char *plaintext = malloc(strlen(ciphertext) + 1);

        // Déchiffrer le texte avec la clé
        decrypt_xor(ciphertext, plaintext, keys[k]);

        // Calculer les fréquences des lettres
        double frequencies[ALPHABET_SIZE];
        calculate_frequencies(plaintext, frequencies);

        // Calculer la distance avec les fréquences théoriques
        double target_frequencies[ALPHABET_SIZE];
        if (strcmp(language, "francais") == 0) {
            memcpy(target_frequencies, french_letter_frequencies, sizeof(french_letter_frequencies));
        } else if (strcmp(language, "anglais") == 0) {
            memcpy(target_frequencies, english_letter_frequencies, sizeof(english_letter_frequencies));
        } else {
            fprintf(stderr, "Langue non supportée\n");
            exit(EXIT_FAILURE);
        }
        double distance = calculate_distance(target_frequencies, frequencies);
        
        // Écrire les résultats dans le fichier
        fprintf(output_file, "Clé: %s, Distance: %.4f\n", keys[k], distance);

        // Vérifier si cette clé est la meilleure
        if (distance < best_distance) {
            best_distance = distance;
            best_key_index = k;
        }

        free(plaintext);
    }

    // Afficher la meilleure clé
    printf("Meilleure clé : %s\n", keys[best_key_index]);

    // Écrire la meilleure clé dans le fichier
    fprintf(output_file, "\nMeilleure clé : %s\n", keys[best_key_index]);

    // Libérer la mémoire
    for (int i = 0; i < num_keys; i++) {
        free(keys[i]);
    }
    free(keys);
    free(ciphertext);

    return 0;
}