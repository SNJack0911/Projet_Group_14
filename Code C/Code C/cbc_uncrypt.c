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
char bloc_clair_dc[LEN_BLOC+1];
char bloc_prec_dc[LEN_BLOC];
char bloc_chiffre_dc[LEN_BLOC];
bool first_bloc = true;


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


/* Programme principal */

int main (int argc, char* argv[]){
  printf("============================\n");
  printf("Lancement ...\n");
  printf("============================\n");
  if (argc <5){
    fprintf(stderr,"Parametres manquants\n");
    return 0;
  }
  printf("============================\n");
  printf("Déchiffrement ...\n");
  int resultat=cbc_uncrypt(argv[1],argv[2],argv[3],argv[4]);
  if (resultat !=0 ){
    return resultat;
  }
  printf("Fin du déchiffrement\n");
  printf("============================\n");
  printf("\nFin \n");
  return 0;
}
