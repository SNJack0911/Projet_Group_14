#include <stdio.h>
#include <string.h>
#include "fonctions_annexes.h"

#define LEN_BLOC 16     // Définir LEN_BLOC si ce n'est pas déjà défini


/* Fonctions annexes partie 1*/

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
