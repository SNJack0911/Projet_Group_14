#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "break_head.h"
#include "ordonered_list.h"
// Prints usage information (help)
static void print_usage(const char *progname) {
    fprintf(stdout, "Usage: %s [OPTIONS]\n", progname);
    fprintf(stdout, "  -i <ciphertext-file> : file to decrypt\n");
    fprintf(stdout, "  -m <method>          : c1 or all\n");
    fprintf(stdout, "  -k <key-length>      : length of key (used for c1)\n");
    fprintf(stdout, "  -d <dictionary>      : dictionary file (used if -m all)\n");
    fprintf(stdout, "  -l <log-file>        : log file to record progress\n");
    fprintf(stdout, "  -h                   : print this help\n\n");
    fprintf(stdout, "Examples:\n");
    fprintf(stdout, "  ./%s -i Datas/Crypted/ringCxor.txt -m c1 -k 4 -l log.txt\n", progname);
    fprintf(stdout, "  ./%s -i Datas/Crypted/ringCxor.txt -m all -k 4 -d Dicos/english.txt\n\n", progname);
}

int main(int argc, char *argv[]) {
    char *input_file = NULL;     // -i
    char *method = NULL;         // -m
    int   key_length = 0;        // -k
    char *dictionary = NULL;     // -d
    char *language = NULL;
    char *log_file = NULL;       // -l
    int   show_help = 0;         // -h
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h")) {
            show_help = 1;
        } else if (!strcmp(argv[i], "-i")) {
            if (++i < argc) {
                input_file = argv[i];
            }
        } else if (!strcmp(argv[i], "-m")) {
            if (++i < argc) {
                method = argv[i];
            }
        } else if (!strcmp(argv[i], "-k")) {
            if (++i < argc) {
                key_length = atoi(argv[i]);
            }
        } else if (!strcmp(argv[i], "-d")) {
            if (++i < argc) {
                dictionary = argv[i];
            }
        } else if (!strcmp(argv[i], "-l")) {
            if (++i < argc) {
                log_file = argv[i];
            }
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    // If help is requested or no arguments
    if (show_help || argc < 2) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    // Basic validation
    if (!input_file) {
        fprintf(stderr, "Error: You must specify an input file with -i\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (!method) {
        fprintf(stderr, "Error: You must specify a method with -m (c1 or all)\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (!strcmp(method, "c1") && key_length <= 0) {
        fprintf(stderr, "Error: A positive key length is required for method c1\n");
        return EXIT_FAILURE;
    }
    if (!strcmp(method, "all") && (key_length <= 0 || !dictionary)) {
        fprintf(stderr, "Error: Method 'all' requires -k <key-length> and -d <dictionary>\n");
        return EXIT_FAILURE;
    }

    // Optional: open a log file if needed (not strictly required)
    FILE *log_fp = NULL;
    if (log_file) {
        log_fp = fopen(log_file, "w");
        if (!log_fp) {
            fprintf(stderr, "Warning: Could not open log file '%s' for writing.\n", log_file);
        }
    }
    printf("Crypted file : %s\n",input_file);
    char *ciphertext = read_file(input_file);
    if (!ciphertext) {
        if (log_fp) fclose(log_fp);
        return EXIT_FAILURE;
    }
    char *dictname;

    // Find the last '/' in the file path
    dictname = strrchr(dictionary, '/');
    if (dictname != NULL) {
        // Move past the '/' to get the filename
        dictname++;
    } else {
        // No '/' found, the entire path is the filename
        dictname = dictionary;
    }
    if (dictionary) {
        if ((strcmp(dictname, "dicoFrSA.txt") == 0)
            || (strcmp(dictname, "liste.de.mots.francais.frgut.txt") == 0)) {
            language = "francais";
        } else if ((strcmp(dictname, "dictionary.txt") == 0)
            || (strcmp(dictname, "english.txt") == 0)) {
            language = "anglais";
        } else {
            fprintf(stderr, "Error: Unknown dictionary '%s'\n", dictionary);
            free(ciphertext);
            if (log_fp) fclose(log_fp);
            return EXIT_FAILURE;
        }
    }

    // Process methods
    if (!strcmp(method, "c1")) {
        exec_c1((const char *)ciphertext, key_length);
        print_list_clef();
    } else if (!strcmp(method, "all")) {
        printf("Attaque brutale avec dico : c1, c2, c3 enchaînées...\n");
        printf("==>dico name = %s\n",dictionary);
        printf("Créé et initialisé\n");
        exec_c1((const char *)ciphertext, key_length);
        exec_c2((const char *)input_file, language);
        exec_c3((const char *)input_file, dictionary);
    } else {
        fprintf(stderr, "Error: Unknown method '%s'\n", method);
        free(ciphertext);
        if (log_fp) fclose(log_fp);
        return EXIT_FAILURE;
    }
    // Cleanup
    free(ciphertext);
    if (log_fp) fclose(log_fp);


    return EXIT_SUCCESS;
}