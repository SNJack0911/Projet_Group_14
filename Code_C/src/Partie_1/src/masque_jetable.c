#include "fonctions_annexes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Function to generate a random key
unsigned char *gen_key_mask(int length) {
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    unsigned char *key = malloc(length + 1);
    if (!key) {
        perror("Failed to allocate memory for key");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < length; i++) {
        key[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    key[length] = '\0';
    return key;
}

void get_key(unsigned char *key, int length) {
    FILE *fd = fopen("key_mask.txt", "r+");  // Open file in read+write mode
    if (fd == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Check if the key file has an existing key
    fseek(fd, 0, SEEK_END);
    long file_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    if (file_size == 0 || file_size < length) {
        // File is empty, generate a new key and write it to the file
        unsigned char *new_key = gen_key_mask(length);
        memcpy(key, new_key, length); 
        fwrite(new_key, sizeof(unsigned char), length, fd);
        fclose(fd);
        free(new_key);
        new_key=NULL;
    }else {
        // File contains data, check if it's the correct length
        size_t bytesRead = fread(key, sizeof(unsigned char), (size_t)length, fd);
        if (bytesRead != (size_t)length) {
            fprintf(stderr, "Error: Key length in file does not match message length.\n");
            fclose(fd);
            exit(EXIT_FAILURE);
        }
        fclose(fd);

        // Truncate the file to clear the key
        fd = fopen("key_mask.txt", "w");  // Reopen in write mode to truncate
        if (fd != NULL) {
            fclose(fd);
        } else {
            perror("Error truncating file");
            exit(EXIT_FAILURE);
        }
    }
}

// Function to perform XOR encryption with a one-time pad (mask jetable)
void mask_jetable(const char *msg, char *crypted, int msg_length, char *key) {
    unsigned char* buffer_key = malloc(msg_length + 1);
    
    // Generate a new key if key is NULL or insufficient length
    if (key == NULL || strlen(key) < (size_t)msg_length) {
        fprintf(stdout, "Error: Input Key or Key File is not the required specification, using a newly generated key\n");
        get_key(buffer_key, msg_length);
        buffer_key[msg_length] = '\0';  // Null-terminate the generated key
        key = (char *)buffer_key;  // Use the generated key
        fprintf(stdout, "New key is store at test_file/key_mask.txt\n");
    }
    // Encrypt the message
    for (int i = 0; i < msg_length; i++) {   
        crypted[i] = msg[i] ^ key[i];
    }
    crypted[msg_length] = '\0';  // Null-terminate
    free(buffer_key);

}

// Function to test the mask_jetable encryption and decryption
void test_mask_jetable(const char *msg) {
    int msg_length = strlen(msg);
    char crypte[msg_length + 1];
    char decrypte[msg_length + 1];

    // Encrypt the message
    printf("Message init : %s\n", msg);
    mask_jetable(msg, crypte,msg_length,"ajawbcjab");
    printf("Message crypte :");

    // Print encrypted message in hexadecimal to make it readable
    for (size_t i = 0; i < strlen(msg); i++) {
        printf("%02x", (unsigned char)crypte[i]);
    }
    printf("\n");
    // Decrypt the message
    mask_jetable(crypte, decrypte,msg_length,"ajawbcjab");
    printf("Message decrypte: %s\n", decrypte);

    // Verify if the decryption matches the original message
    if (strcmp(msg, decrypte) == 0) {
        printf("Mask Test Valide: Decryption est le meme que message init.\n");
    } else {
        printf("Mask Test Non-valide: Decryption mauvais\n");
    }
}

// int main(int argc, char *argv[]) {
//     if (argc >2){
//         fprintf(stderr,"Too much arg");
//         return -1;
//     }
//     const char *msg = argv[1]; 
//     test_mask_jetable(msg);
//     return 0;
// }
