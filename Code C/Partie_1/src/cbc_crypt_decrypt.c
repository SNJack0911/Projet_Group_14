#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fonctions_annexes.h"

#define LEN_BLOC 16

/* Global variables */

FILE *plaintext_file;
FILE *encrypted_file;
char plaintext_block[LEN_BLOC+1];
char decrypted_plaintext_block[LEN_BLOC+1];
char previous_block[LEN_BLOC+1];
char previous_decrypted_block[LEN_BLOC+1];
char encrypted_block[LEN_BLOC+1];
char decrypted_encrypted_block[LEN_BLOC+1];
bool is_first_block = true;

/* Functions for method 2 */

void format_key(char * key) {
  /* Function to take a string and transform it
  into a string of size LEN_BLOC by either truncating or
  repeating the string. */

  int key_length = strlen(key);
  char new_key[LEN_BLOC+1];
  for (int i = 0; i < LEN_BLOC; i++) {
    new_key[i] = key[i % key_length];
  }
  new_key[LEN_BLOC] = '\0';
  memcpy(key, new_key, LEN_BLOC+1);
}

void display_block_hex(const char *block, size_t size) {
    // Function to transform a string into a hexadecimal code

    printf("Block: ");
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", (unsigned char)block[i]);
    }
    printf("\n");
}

/* Encryption functions */

int write_encrypted_message() {
    // Direct writing of encrypted bytes into the binary file

    if (fwrite(encrypted_block, 1, LEN_BLOC, encrypted_file) != LEN_BLOC) {
        perror("Error writing to file");
        return 6;
    }
    return 0;
}

void encrypt_message(char *vector, char *key) {
    // Encrypt the plaintext block using XOR encryption method

    char middle_block[LEN_BLOC];
    int i;
    if (is_first_block) {
        is_first_block = false;
        for (i = 0; i < LEN_BLOC; i++) {
            middle_block[i] = vector[i] ^ plaintext_block[i];
        }
    } else {
        for (i = 0; i < LEN_BLOC; i++) {
            middle_block[i] = previous_block[i] ^ plaintext_block[i];
        }
    }
    for (int i = 0; i < LEN_BLOC; i++) {
        encrypted_block[i] = key[i] ^ middle_block[i];
    }
}

int cbc_encrypt(char *plaintext_filename, char *vector, char *key, char *encrypted_filename) {
    /* Main encryption function

    Parameters:
    plaintext_filename: file containing plaintext to be encrypted,
    vector: initialization vector of size 16 bytes,
    key: encryption key,
    encrypted_filename: file where the encrypted content will be written.

    This function opens the plaintext file and reads its content block by block (16 bytes).
    Each block is encrypted using the encrypt_message function
    and written to the encrypted file.
    */

    printf("Opening files\n");
    plaintext_file = fopen(plaintext_filename, "rb");
    encrypted_file = fopen(encrypted_filename, "wb");

    printf("Files opened\n");
    size_t bits_read;
    is_first_block = true;

    if (!plaintext_file || !encrypted_file) {
        if (plaintext_file) fclose(plaintext_file);
        if (encrypted_file) fclose(encrypted_file);
        perror("Error opening file");
        return 1;
    }

    if (strlen(key) < LEN_BLOC) {
        format_key(key);
    }
    printf("Key: %s\n", key);

    while ((bits_read = fread(plaintext_block, 1, LEN_BLOC, plaintext_file)) > 0) {
        if (bits_read < LEN_BLOC) {
            if (plaintext_block[bits_read - 1] == '\n') {
                plaintext_block[bits_read - 1] = ' ';
            }
            memset(plaintext_block + bits_read, ' ', LEN_BLOC - bits_read);
        }
        plaintext_block[LEN_BLOC] = '\0';
        encrypt_message(vector, key);
        memcpy(previous_block, encrypted_block, LEN_BLOC);
        int result;
        if ((result = write_encrypted_message()) != 0) {
            return result;
        }
    }
    fclose(plaintext_file);
    fclose(encrypted_file);
    return 0;
}

/* Decryption functions */

void decrypt_message(char *vector, char *key) {
    // Decrypt the encrypted block using XOR decryption method

    char middle_decrypted_block[LEN_BLOC];
    int i;

    for (i = 0; i < LEN_BLOC; i++) {
        middle_decrypted_block[i] = decrypted_encrypted_block[i] ^ key[i];
    }

    if (is_first_block) {
        is_first_block = false;
        for (i = 0; i < LEN_BLOC; i++) {
            decrypted_plaintext_block[i] = middle_decrypted_block[i] ^ vector[i];
        }
    } else {
        for (i = 0; i < LEN_BLOC; i++) {
            decrypted_plaintext_block[i] = middle_decrypted_block[i] ^ previous_decrypted_block[i];
        }
    }
}

int write_decrypted_message() {
    // Direct writing of decrypted bytes into the binary file

    decrypted_plaintext_block[LEN_BLOC] = '\0';
    if (fwrite(decrypted_plaintext_block, 1, LEN_BLOC, plaintext_file) != LEN_BLOC) {
        perror("Error writing to file");
        return 1;
    }
    return 0;
}

int cbc_decrypt(char *encrypted_filename, char *vector, char *key, char *decrypted_filename) {
    /* Main decryption function

    Parameters:
    encrypted_filename: file containing encrypted content to be decrypted,
    vector: initialization vector of size 16 bytes,
    key: decryption key identical to the one used for encryption,
    decrypted_filename: file where the decrypted content will be written.

    This function opens the encrypted file and reads its content block by block (16 bytes).
    Each block is decrypted using the decrypt_message function
    and written to the decrypted file.
    */

    printf("Opening files\n");
    encrypted_file = fopen(encrypted_filename, "rb");
    plaintext_file = fopen(decrypted_filename, "wb");
    printf("Files opened\n");
    size_t bits_read;
    is_first_block = true;

    if (!encrypted_file || !plaintext_file) {
        fclose(encrypted_file);
        fclose(plaintext_file);
        perror("Error opening file");
        return 2;
    }

    if (strlen(key) < LEN_BLOC) {
        format_key(key);
    }
    printf("Key: %s\n", key);

    while ((bits_read = fread(decrypted_encrypted_block, 1, LEN_BLOC, encrypted_file)) > 0) {
        if (bits_read != LEN_BLOC) {
          perror("Error reading file in cbc_decrypt");
          return 9;
        }

        decrypt_message(vector, key);
        memcpy(previous_decrypted_block, decrypted_encrypted_block, LEN_BLOC);
        int result;
        if ((result = write_decrypted_message()) != 0) {
            return result;
        }
    }

    fclose(encrypted_file);
    fclose(plaintext_file);
    return 0;
}

void test_cbc() {
    // Test function for the CBC encryption/decryption method

    char *key = (char *)gen_key(LEN_BLOC);
    printf("Key: %s\n", key);
    char vector[LEN_BLOC] = "abcdefghijklmnop";

    // Encryption
    printf("CBC Encryption for file: msg1.txt\n");
    cbc_encrypt("test_files/msg1.txt", vector, key, "encrypted_files/cbc_msg1_encrypt.txt");

    // Decryption
    cbc_decrypt("encrypted_files/cbc_msg1_encrypt.txt", vector, key, "decrypted_files/cbc_decrypt_msg1.txt");

    // Verification of decryption
    FILE *original_file = fopen("test_files/msg1.txt", "r");
    FILE *decrypted_file = fopen("decrypted_files/cbc_decrypt_msg1.txt", "r");
    if (!original_file || !decrypted_file) {
        perror("Error opening file");
        return;
    }

    char original_buffer[256];
    char decrypted_buffer[256];

    if (fgets(original_buffer, 256, original_file) == NULL) {
        perror("Error reading original file");
        fclose(original_file);
        fclose(decrypted_file);
        free(key);
        key = NULL;
        return;
    }

    if (fgets(decrypted_buffer, 256, decrypted_file) == NULL) {
        perror("Error reading decrypted file");
        fclose(original_file);
        fclose(decrypted_file);
        free(key);
        key = NULL;
        return;
    }

    if (strcmp(original_buffer, decrypted_buffer) == 0) {
        printf("CBC Test Passed: Decryption matches original message.\n");
    } else {
        printf("CBC Test Failed: Decryption does not match original message.\n");
    }

    fclose(original_file);
    fclose(decrypted_file);
    free(key);
    key = NULL;
}

// int main(void){
//     test_cbc();
//     return 0;
// }

