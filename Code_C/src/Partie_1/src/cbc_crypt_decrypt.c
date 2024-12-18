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
char bloc_clair_dc[LEN_BLOC+1];
char bloc_prec[LEN_BLOC+1];
char bloc_prec_dc[LEN_BLOC+1];
char bloc_chiffre[LEN_BLOC+1];
char bloc_chiffre_dc[LEN_BLOC+1];
bool first_bloc = true;

/* Fonctions pour la methode 2*/

void formater_key(char * key){
  /* Fonction prennant une chaine de caractère et la transforme 
  en une chaine de taille LEN_BLOC soit en troquant ou en 
  répétant la la chaine de caractère. */
  
  int taille_key = strlen(key);
  char new_key[LEN_BLOC+1];
  for (int i =0; i< LEN_BLOC ; i++){
    new_key[i]=key[i%taille_key];
  }
  new_key[LEN_BLOC] = '\0';
  memcpy(key,new_key,LEN_BLOC+1);
}

void afficher_bloc_hex(const char *bloc, size_t taille) {
    // Fonction transformant une chaine de caractère en un code hexadécimal
  
    printf("Bloc : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%02X ", (unsigned char)bloc[i]);
    }
    printf("\n");
}

/* Fonctions cryptage */


int construct_crypt_message(){
    // Écriture directe des octets chiffrés dans le fichier binaire
    
    // afficher_bloc_hex(bloc_chiffre, LEN_BLOC);
    if (fwrite(bloc_chiffre, 1, LEN_BLOC, message_chiffre) != LEN_BLOC) {
        perror("Erreur write");
        return 6;
    }
    return 0;
}

void chiffrer_message(char * vecteur,char * key){
    // Cryptage du bloc clair avec la méthode de chiffrage XOR
    
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
    /* Fonction principale de cryptage 

    Param
    fichier_clair : fichier clair dont le contenu va être crypté,
    vecteur : vecteur d'initialisation de taille 16o,
    key : clé de  chiffrement,
    fichier_chiffre : fichier dans lequel sera écrit le contenu crypté,

    Cette fonction ouvre fichier_clair et litson contenu, par bloc de 16o.
    A chaque lecture, le bloc lu va être crypté par la fonction chiffrer_message 
    et écrit dans fichier_chiffre
    */
    
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
        // printf("bloc clair : %s", bloc_clair);
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

/* Fonctions décryptage */

void dechiffrer_message(char *vecteur, char *key){
    // Decryptage du bloc chiffre avec la méthode de chiffrage XOR
    
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
    // Écriture directe des octets déchiffrés dans le fichier binaire
    
    bloc_clair_dc[LEN_BLOC] = '\0';
    // printf("Bloc clair : %s\n", bloc_clair_dc);
    if (fwrite(bloc_clair_dc, 1, LEN_BLOC, message_clair) != LEN_BLOC) {
        perror("Erreur write");
        return 1;
    }
    return 0;
}

int cbc_uncrypt(char *fichier_chiffre, char *vecteur, char *key, char *fichier_dechiffre) {
    /* Fonction principale de décryptage 

    Param
    fichier_chiffre : fichier crypté dont le contenu va être décrypté,
    vecteur : vecteur d'initialisation de taille 16o,
    key : clé de déchiffrement identique à celle utilisée pour le chiffrement ,
    fichier_dechiffre : fichier dans lequel sera écrit le contenu décrypté,

    Cette fonction ouvre fichier_chiffre et lit son contenu, par bloc de 16o.
    A chaque lecture, le bloc lu va être décrypter par la fonction dechiffrer_message 
    et écrit dans fichier_dechiffre
    */
    
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
        // printf("bloc chiffré : %s\n", bloc_chiffre_dc);
        // afficher_bloc_hex(bloc_chiffre_dc,LEN_BLOC);

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
void test_cbc(){
    // Fonction de test de la méthode de chiffrement/déchiffrement CBC
    
    char *key = (char *)gen_key(LEN_BLOC);
    printf("Key : %s\n", key);
    char vecteur[LEN_BLOC] = "abcdefghijklmnop";
    
    // Cryptage
    printf("Cryptage CBC fiche : msg1.txt\n");
    cbc_crypt("test_files/msg1.txt", vecteur, key, "encrypted_files/cbc_msg1_crypt.txt");
    
    // Décryptage
    cbc_uncrypt("encrypted_files/cbc_msg1_crypt.txt", vecteur, key, "decrypted_files/cbc_decrypt_msg1.txt");
    
    // Vérification du déchiffrement
    FILE *fichier_clair = fopen("test_files/msg1.txt", "r");
    FILE *fichier_dechiffre = fopen("decrypted_files/cbc_decrypt_msg1.txt", "r");
    if (!fichier_clair || !fichier_dechiffre) {
        perror("Erreur ouverture fichier");
        return;
    }
    
    char buffer_clair[256];
    char buffer_dechiffre[256];
    // Read the original (plaintext) file
    if (fgets(buffer_clair, 256, fichier_clair) == NULL) {
        perror("Erreur de lecture du fichier original");
        fclose(fichier_clair);
        fclose(fichier_dechiffre);
        free(key);
        key = NULL;
        return;
    }

    // Read the decrypted file
    if (fgets(buffer_dechiffre, 256, fichier_dechiffre) == NULL) {
        perror("Erreur de lecture du fichier déchiffré");
        fclose(fichier_clair);
        fclose(fichier_dechiffre);
        free(key);
        key = NULL;
        return;
}
    
    if (strcmp(buffer_clair, buffer_dechiffre) == 0) {
        printf("Test CBC Valide: Déchiffrement identique au message initial.\n");
    } else {
        printf("Test CBC Non-valide: Déchiffrement incorrect.\n");
    }
    
    fclose(fichier_clair);
    fclose(fichier_dechiffre);
    free(key);
    key = NULL;
}
// int main(void){
//     test_cbc();
//     return 0;
// }

