#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fonctions_annexes.h"

void xor_encrypt_decrypt(unsigned char *msg, unsigned char* key, char *crypted){
  /// \brief chiffre un message avec une clé par un xor
  /// \param[in] msg : le message à chiffrer
  /// \param[in] key : la clé de chiffrement
  /// \param[out] crypted : le message chiffré
    int len_msg = strlen((char*)msg);
    int len_key = strlen((char*)key);
    int index_key = 0;
    for (int i = 0; i < len_msg; i++){    
        crypted[i] = msg[i] ^ key[index_key];
        index_key = (index_key + 1) % len_key;
    }
}

unsigned char *gen_key (int len){
  /// \brief génère une clé aléatoire de longueur len
  /// \param[out] key : la clé générée
  /// \param[in] len : la longueur de la clé
    srand(time(NULL));   // Initialization, should only be called once.
    unsigned char *key = malloc(len * sizeof(unsigned char));
    for (int i = 0; i < len; i++){
        key[i] = rand() % 256;
    }
    return key;
}
void mask_encrypt(const char *input_file, const char *output_file, const char *key) {
    FILE *in_file = fopen(input_file, "r");
    FILE *out_file = fopen(output_file, "w");

    if (in_file == NULL || out_file == NULL) {
        perror("Error: Could not open file(s)");
        exit(1);
    }

    char ch;
    size_t key_len = strlen(key);
    size_t key_index = 0;

    while ((ch = fgetc(in_file)) != EOF) {
        // XOR the input character with the corresponding key character
        char encrypted_char = ch ^ key[key_index];
        fputc(encrypted_char, out_file);

        // Move to the next key character (looping if necessary)
        key_index = (key_index + 1) % key_len;
    }

    fclose(in_file);
    fclose(out_file);
}
int main(int argc, char const *argv[])
{
  if (argc != 3){
    printf("Usage: %s <message> <key> \n", argv[0]);
    return 1;
  }
  unsigned char *msg = (unsigned char *)argv[1];
  int key_seed = atoi(argv[2]);
  char crypted[strlen((char*)msg)];
  unsigned char *key = gen_key(key_seed);
  printf("Clé générée : %s\n",key);
  xor_encrypt_decrypt(msg, key, crypted);
  printf("Message chiffré : %s\n", crypted);
  xor_encrypt_decrypt((unsigned char*)crypted, key, crypted);
  printf("Message déchiffré : %s\n", crypted);
  return 0;
}
