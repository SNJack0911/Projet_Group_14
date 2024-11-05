#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fonctions_annexes.h"

void xor_encrypt_decrypt(const char *msg, unsigned char* key, char *crypted, int len_msg) {

    int len_key = strlen((char*)key);
    int index_key = 0;
    for (int i = 0; i < len_msg; i++){    
        crypted[i] = msg[i] ^ key[index_key];
        index_key = (index_key + 1) % len_key;
    }
}
unsigned char *gen_key (int len){

    srand(time(NULL));   // Init rand, called 1 time only.
    unsigned char *key = malloc(len * sizeof(unsigned char));
    for (int i = 0; i < len; i++){
        key[i] = rand() % 256;
    }
    return key;
}
void test_xor() {
    unsigned char *key = gen_key(15); // Exemple key
    printf("Key : %s\n",key);
    const char *texte = "Bonjour, ici c'est un message secret."; // Exemple message
    char crypte[256];
    char decrypte[256];
    
    // Crypt
    printf("Message init : %s\n", texte);
    xor_encrypt_decrypt(texte, key, crypte, strlen(texte));
    printf("Message crypte : %s\n", crypte);
    
    // Decrypt 
    xor_encrypt_decrypt(crypte, key, decrypte, strlen(texte));
    printf("Message decrypte: %s\n", decrypte);

    // Verify the decryption matches the original plaintext
    if (strcmp(texte, decrypte) == 0) {
        printf("XOR Test Valide: Decryption est le meme que message init.\n");
    } else {
        printf("XOR Test Non-valide: Decryption mauvais\n");
    }
    free(key);
    key=NULL;
}
// int main (void) {
//     test_xor();
//     return 0;
// }

