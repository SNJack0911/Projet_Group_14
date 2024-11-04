#ifndef FONCTIONS_ANNEXES_H
#define FONCTIONS_ANNEXES_H

#include <stddef.h>  // Pour size_t

void formater_key(char * key);
void afficher_bloc_hex(const char *bloc, size_t taille);

// Les fonctions pour la methode 1 de la partie 1
void xor_encrypt_decrypt(const char *msg, unsigned char* key, char *crypted, int len_msg);
unsigned char *gen_key (int len);

// Les fonctions pour la methode mask jetable
void mask_jetable (const char *msg,char *crypted,int key_length, char *key);

// Les fonctions pour la methode 2 de la partie 1
int cbc_crypt(char *fichier_clair, char * vecteur, char * key, char * fichier_chiffre);
int cbc_uncrypt(char *fichier_chiffre, char *vecteur, char *key, char *fichier_dechiffre);

//Les fonctions de test
void test_xor();
void test_masque_jetable(const char *msg );

#endif
