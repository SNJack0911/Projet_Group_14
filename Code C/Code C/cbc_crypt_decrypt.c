#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fonctions_annexes.h"

#define LEN_BLOC 16


/* Variables globales */

FILE *message_clair;
FILE *message_chiffre;
char bloc_clair[LEN_BLOC+1];
char bloc_clair_dc[LEN_BLOC];
char bloc_prec[LEN_BLOC];
char bloc_prec_dc[LEN_BLOC];
char bloc_chiffre[LEN_BLOC];
char bloc_chiffre_dc[LEN_BLOC];
bool first_bloc = true;


/* Fonctions cryptages */

int construct_crypt_message(){
    // Affichage du bloc chiffré en hexadécimal pour le débogage (facultatif)
    afficher_bloc_hex(bloc_chiffre, LEN_BLOC);
    // Écriture directe des octets chiffrés dans le fichier binaire
    if (fwrite(bloc_chiffre, 1, LEN_BLOC, message_chiffre) != LEN_BLOC) {
        perror("Erreur write");
        return 6;
    }

    return 0;
}

void chiffrer_message(char * vecteur,char * key){
    char bloc_middle[LEN_BLOC];
    int i;
    if(first_bloc){
        first_bloc = false;
        for(i = 0; i < LEN_BLOC; i++){
            bloc_middle[i] = vecteur[i] ^ bloc_clair[i];
        }
    }else{
        for(i = 0; i < LEN_BLOC; i++){
            bloc_middle[i] = bloc_prec[i] ^ bloc_clair[i];
        }
    }
    for (int i = 0; i < LEN_BLOC; i++) {
        bloc_chiffre[i] = key[i] ^ bloc_middle[i];
    }
}

int cbc_crypt(char *fichier_clair, char * vecteur, char * key, char * fichier_chiffre) {
    printf("Ouverture fichiers\n");
    message_clair = fopen(fichier_clair, "rb");
    message_chiffre = fopen(fichier_chiffre, "wb");

    printf("Fichiers ouverts\n");
    size_t bits_lus;
    first_bloc = true;

    if (!message_clair || !message_chiffre) {
        if (message_clair) fclose(message_clair);  // Ferme seulement si le fichier est ouvert
        if (message_chiffre) fclose(message_chiffre);
        perror("Erreur ouverture fichier");
        return 1;
    }


    if(strlen(key) < LEN_BLOC){
        formater_key(key);
    }
    printf("Ma clé : %s\n",key);

    while ((bits_lus = fread(bloc_clair, 1, LEN_BLOC, message_clair)) > 0) {
        if (bits_lus < LEN_BLOC) {
            if(bloc_clair[bits_lus-1] == '\n'){
                bloc_clair[bits_lus-1] = ' ';
            }
            memset(bloc_clair + bits_lus, ' ', LEN_BLOC - bits_lus);
        }
        bloc_clair[LEN_BLOC] = '\0';
        printf("bloc clair : %s", bloc_clair);
        chiffrer_message(vecteur,key);
        memcpy(bloc_prec, bloc_chiffre, LEN_BLOC); //copie bloc_chiffre dans bloc_prec
        int result;
        if((result = construct_crypt_message()) != 0){
            return result;
        }
    }
    fclose(message_clair);
    fclose(message_chiffre);
    return 0;
}

/* Fonctions déchiffrements */

void dechiffrer_message(char *vecteur, char *key){
    char bloc_middle_dc[LEN_BLOC];
    int i;

    // XOR avec la clé pour obtenir le bloc middle
    for (i = 0; i < LEN_BLOC; i++) {
        bloc_middle_dc[i] = bloc_chiffre_dc[i] ^ key[i];
    }

    // Si c'est le premier bloc, XOR avec le vecteur d'initialisation
    if (first_bloc) {
        first_bloc = false;
        for (i = 0; i < LEN_BLOC; i++) {
            bloc_clair_dc[i] = bloc_middle_dc[i] ^ vecteur[i];
        }
    } else {
        // Pour les blocs suivants, XOR avec le bloc chiffré précédent
        for (i = 0; i < LEN_BLOC; i++) {
            bloc_clair_dc[i] = bloc_middle_dc[i] ^ bloc_prec_dc[i];
        }
    }
}

int construct_decrypt_message() {
    bloc_clair_dc[LEN_BLOC] = '\0';
    printf("Bloc clair : %s\n", bloc_clair_dc);
    if (fwrite(bloc_clair_dc, 1, LEN_BLOC, message_clair) != LEN_BLOC) {
        perror("Erreur write");
        return 1;
    }
    return 0;
}

int cbc_uncrypt(char *fichier_chiffre, char *vecteur, char *key, char *fichier_dechiffre) {
    printf("Ouverture fichiers\n");
    message_chiffre = fopen(fichier_chiffre, "rb");
    message_clair = fopen(fichier_dechiffre, "wb");
    printf("Fichiers ouverts\n");
    size_t bits_lus;
    first_bloc = true;

    if (!message_chiffre || !message_clair) {
        fclose(message_chiffre);
        fclose(message_clair);
        perror("Erreur ouverture fichier");
        return 2;
    }

    if (strlen(key) < LEN_BLOC) {
        formater_key(key);
    }
    printf("Ma clé : %s\n", key);

    while ((bits_lus = fread(bloc_chiffre_dc, 1, LEN_BLOC, message_chiffre)) > 0) {
        if (bits_lus != LEN_BLOC) {
          perror("Erreur fread cbc_decrypt");
          return 9;
        }
        printf("bloc chiffré : %s\n", bloc_chiffre_dc);
        afficher_bloc_hex(bloc_chiffre_dc,LEN_BLOC);

        dechiffrer_message(vecteur, key);

        memcpy(bloc_prec_dc, bloc_chiffre_dc, LEN_BLOC); // sauvegarde du bloc chiffré
        int resultat;
        if ((resultat = construct_decrypt_message()) != 0) {
            return resultat;
        }
    }

    fclose(message_chiffre);
    fclose(message_clair);
    return 0;
}


