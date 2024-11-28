#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>


#include "dh_prime.h"

// Fonction pour afficher l'aide
void print_help() {
    printf("Usage: dh_gen_group [options]\n");
    printf("Options:\n");
    printf("  -o <filename>  Specify output file to write p and g.\n");
    printf("  -h             Display this help message.\n");
}

// Fonction principale
int main(int argc, char *argv[]) {
    long min = MIN_PRIME; // Valeurs minimum et maximum par défaut pour le nombre premier
    long max = MAX_PRIME;
    char *output_file =  NULL;
    int opt;

    while ((opt = getopt(argc, argv, "o:h")) != -1) {
        switch (opt) {
            case 'o':
                output_file = optarg;
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
    FILE *output_fd = fopen(output_file, "w");
    if (!output_fd) {
        perror("Error opening output file");
        print_help();
        return -1;
    }
    generate_shared_key(min, max, output_fd);
    fclose(output_fd);
    return 0;

}
