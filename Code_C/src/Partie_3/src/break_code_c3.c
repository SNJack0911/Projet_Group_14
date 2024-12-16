#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "break_head.h"

void exec_c3(const char *ciphertext_file,const char *dictionary) {
    printf("Début de C3...\n");
    const char *keys_file = "/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/best_key_c2.txt";
    // Lire les clés depuis le fichier
    int keys_count = 0;
    char **keys_tab = read_keys(keys_file, &keys_count);
    if (keys_tab == NULL) {
        return;  // Erreur lors de la lecture des clés
    }
    if (keys_count > MAX_KEYS_COUNT) {
        keys_count = MAX_KEYS_COUNT;
    }

    // Déchiffrer et chercher la meilleure clé
    char * best_key=break_code_c3((const char **)keys_tab, keys_count, dictionary, ciphertext_file);
    if (!best_key){
        exit(1);
    }

    // Libérer la mémoire des clés
    for (int i = 0; i < keys_count; i++) {
        free(keys_tab[i]);
    }
    free(keys_tab);
    printf("Fin de C3 : meilleure clef = %s\n",best_key);
    FILE*fd = fopen("/home/hiraichi/work/Projet_Group_14/Code_C/Util/Key/crack_key.txt", "w");
    fprintf(fd,"%s",best_key);
    fclose(fd); 
    free (best_key);
}
