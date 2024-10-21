#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <getopt.h>

#include "fonctions_annexes.h"


// Function to display help message
void print_help() {
    printf("Usage: sym_crypt -i <input_file> -o <output_file> -k <key> [-g <key_length>]\n");
    printf("Options:\n");
    printf("  -i <input_file>    : File to encrypt/decrypt\n");
    printf("  -o <output_file>   : Output file for encrypted/decrypted message\n");
    printf("  -k <key>           : Symmetric key to use for XOR encryption/decryption\n");
    printf("  -g <key_length>    : Generate a random key of specified length\n");
}

int main(int argc, char *argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char *key = NULL;
    char *key_file = NULL;
    char *method = NULL;
    char *vector_file = NULL;

    while ((opt = getopt(argc, argv, "i:o:k:f:m:v:h")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'k':
                key = optarg;
                break;
            case 'f':
                key_file = optarg;
                break;
            case 'm':
                method = optarg;
                break;
            case 'v':
                vector_file = optarg;
                break;
            case 'h':
                print_help();
                return 0;
            default:
                fprintf(stderr, "Invalid option\n");
                print_help();
                return 1;
        }
    }

    if (!input_file || !output_file || (!key && !key_file) || !method) {
        fprintf(stderr, "Missing required arguments\n");
        print_help();
        return 1;
    }

    // Read key from file if -f is used
    if (key_file) {
        FILE *kf = fopen(key_file, "r");
        if (kf == NULL) {
            fprintf(stderr, "Failed to open key file\n");
            return 1;
        }
        // Assuming the key is stored in one line in the file
        size_t len = 0;
        getline(&key, &len, kf);
        fclose(kf);
    }

    // Open input file
    FILE *input = fopen(input_file, "r");
    if (input == NULL) {
        fprintf(stderr, "Failed to open input file\n");
        return 1;
    }

    // Open output file
    FILE *output = fopen(output_file, "w");
    if (output == NULL) {
        fprintf(stderr, "Failed to open output file\n");
        fclose(input);
        return 1;
    }

    // Perform encryption/decryption based on the method
    if (strcmp(method, "xor") == 0) {
        xor_encrypt_decrypt((unsigned char *)input_file, (unsigned char*)key,output_file);
    } else if (strcmp(method, "cbc-crypt") == 0 || strcmp(method, "cbc-uncrypt") == 0) {
        if (!vector_file) {
            fprintf(stderr, "vector file required for CBC methods\n");
            fclose(input);
            fclose(output);
            return 1;
        }
        if (strcmp(method, "cbc-crypt") == 0) {
            cbc_crypt(input_file, vector_file, key, output_file);
        } else {
            cbc_uncrypt(input_file, vector_file, key, output_file);
        }
    } else if (strcmp(method, "mask") == 0) {
        mask_encrypt(input_file, output_file, key);
    } else {
        fprintf(stderr, "Unknown method\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    // Close files
    fclose(input);
    fclose(output);

    return 0;
}