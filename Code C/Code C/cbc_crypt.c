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
char bloc_prec[LEN_BLOC];
char bloc_chiffre[LEN_BLOC];
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
  printf("Test du chiffrement ...\n");
  int resultat = cbc_crypt(argv[1],argv[2],argv[3],argv[4]);
  if (resultat !=0 ){
    return resultat;
  }
  printf("Fin test du chiffrement\n");
}
