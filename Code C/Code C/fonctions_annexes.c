#include <stdio.h>
#include <string.h>
#include "fonctions_annexes.h"

#define LEN_BLOC 16     // Définir LEN_BLOC si ce n'est pas déjà défini


/* Fonctions annexes partie 1*/

void formater_key(char * key){
  int taille_key = strlen(key);
  char new_key[LEN_BLOC+1];
  for (int i =0; i< LEN_BLOC ; i++){
    new_key[i]=key[i%taille_key];
  }
  new_key[LEN_BLOC] = '\0';
  memcpy(key,new_key,LEN_BLOC+1);
}

void afficher_bloc_hex(const char *bloc, size_t taille) {
    printf("Bloc : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%02X ", (unsigned char)bloc[i]);
    }
    printf("\n");
}
