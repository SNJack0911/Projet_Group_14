#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>

#include "fonctions_annexes.h"
 // Function to display help message
void print_help() {
    printf("Usage: sym_crypt -i <input_file> -o <output_file> -k <key> -m method\n");
    printf("Options:\n");
    printf("  -i <input_file>    : File to encrypt/decrypt\n");
    printf("  -o <output_file>   : Output file for encrypted/decrypted message\n");
    printf("  -k <key>           : Symmetric key to use for XOR encryption/decryption\n");
    printf("  -m <method>       : Encryption/decryption method (xor, mask, cbc-crypt, cbc-uncrypt)\n");
}

char *read_info_from_file(int file, int *file_size) {
    // Get file size using lseek
    *file_size = (int)lseek(file, 0, SEEK_END);
    if (*file_size == -1) {
        perror("Error determining file size");
        return NULL;
    }

    // Reset file offset to the beginning
    if (lseek(file, 0, SEEK_SET) == -1) {
        perror("Error resetting file position");
        return NULL;
    }


    // Allocate buffer to hold the file content, plus one for null-termination
    char *buffer = malloc(*file_size + 1);
    if (!buffer) {
        perror("Error allocating memory");
        return NULL;
    }

    // Read the entire file content into the buffer
    if (read(file, buffer, (size_t)*file_size) == -1) {
        perror("Error reading file");
        free(buffer);
        return NULL;
    }

    // Null-terminate the buffer to make it a valid C string
    buffer[*file_size] = '\0';
    // printf("File size: %d bytes\n", *file_size);

    // // Print file content in hexadecimal format for debugging purposes
    // printf("File content in hexadecimal:\n");
    // for (int i = 0; i < *file_size; i++) {
    //     printf("%02x ", (unsigned char)buffer[i]);
    // }
    // printf("\n");

    return buffer;
}

int main(int argc, char *argv[]) {
    // Variable declarations and argument parsing 
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char *key = NULL;
    char *key_file = NULL;
    char *method = NULL;
    char *vector_file = NULL;
    int input;
    int output;
    while ((opt = getopt(argc, argv, "i:o:k:f:m:v:l:h")) != -1) {
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
            case 'l':
                // Ignore this option, it is used for testing purposes
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
        int kf = open(key_file, O_RDWR);
        if (kf == -1) {
            fprintf(stderr, "Failed to open key file\n");
            return 1;
        }
        int key_length;
        char* buffer_key = read_info_from_file(kf, &key_length);
        if (!buffer_key) {
            close(kf);
            return 1;
        }
        key = malloc(key_length+1);
        memcpy(key, buffer_key, key_length); 
        free(buffer_key);
        close(kf);
    }

    // Open input file
    if ((input = open(input_file, O_RDONLY)) == -1) {
        fprintf(stderr, "Failed to open input file\n");
        return 1;
    }

    // Open output file
    if ((output = open(output_file, O_WRONLY|O_CREAT,S_IRUSR | S_IWUSR)) == -1) {
        fprintf(stderr, "Failed to open output file\n");
        close(input);
        return 1;
    }

    // Perform encryption/decryption based on the method
    if (strcmp(method, "xor") == 0 || strcmp(method, "mask") == 0) {
        int buffer_length;
        const char *buffer = read_info_from_file(input,&buffer_length);
        if (!buffer) {
            close(input);
            close(output);
            return 1;
        }
        char output_buffer[buffer_length];
        if (strcmp(method, "mask") == 0) {
            mask_jetable(buffer, output_buffer, buffer_length, key);
        } else {
            xor_encrypt_decrypt(buffer, (unsigned char*)key, output_buffer, buffer_length);
        }

        // printf("Output buffer content in hexadecimal:\n");
        // for (int i = 0; i < buffer_length; i++) {
        //     printf("%02x ", (unsigned char)output_buffer[i]);
        // }
        // printf("\n");

        if (write(output,output_buffer , (size_t)buffer_length) ==-1) {
            perror("Error writing encrypted data to file");
            free((char *)buffer);
            close(input);
            close(output);
            return 1;
        }
        free((char *)buffer);
    } else if (strcmp(method, "cbc-crypt") == 0 || strcmp(method, "cbc-uncrypt") == 0) {
        if (!vector_file) {
            fprintf(stderr, "Vector file required for CBC methods\n");
            close(input);
            close(output);
            return 1;
        }
        if (strcmp(method, "cbc-crypt") == 0) {
            cbc_crypt(input_file, vector_file, key, output_file);
        } else {
            cbc_uncrypt(input_file, vector_file, key, output_file);
        }
    } else {
        fprintf(stderr, "Unknown method\n");
        close(input);
        close(output);
        return 1;
    }

    // Close files in main
    close(input);
    close(output);

    return 0;
}
