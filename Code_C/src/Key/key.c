#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_KEY_LEN 256
#define KEYS_FILE "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Key/keys.txt"
#define USED_KEYS_FILE "/home/hiraichi/work/Projet_Group_14/Code_C/Util/Key/used_keys.txt"

//generate a random key
unsigned char *gen_key (int len){

    srand(time(NULL));   // Init rand, called 1 time only.
    unsigned char *key = malloc(len * sizeof(unsigned char));
    for (int i = 0; i < len; i++) {
        int category = rand() % 3; // Randomly pick a category (0, 1, 2)
        if (category == 0) {
            key[i] = '0' + (rand() % 10); // Numbers '0'-'9'
        } else if (category == 1) {
            key[i] = 'a' + (rand() % 26); // Lowercase letters 'a'-'z'
        } else {
            key[i] = 'A' + (rand() % 26); // Uppercase letters 'A'-'Z'
        }
    }
    key[len] = '\0'; // Null-terminate the string
    return key;
}
// Append key to a file
void append_to_file(const char *filename, const char *key) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\n", key);
    fclose(file);
}
void delete_key_from_file(const char *filename, const char *key, int is_used_keys) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    FILE *temp = tmpfile();
    if (!temp) {
        perror("tmpfile");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char line[MAX_KEY_LEN];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline
        // Check if the line matches the key to delete
        if (strcmp(line, key) != 0) {
            // For used keys file, also check against "Used_<key>"
            if (!(is_used_keys && strncmp(line, "Used_", 5) == 0 && strcmp(line + 5, key) == 0)) {
                printf("line: %s\n", line);
                fprintf(temp, "%s\n", line); // Keep this line if no match
            } else {
                found = 1; // Match found for "Used_<key>"
            }
        } else {
            found = 1; // Match found for normal key
        }
    }

    fclose(file);

    if (!found) {
        fprintf(stderr, "Key '%s' not found in %s.\n", key, filename);
    }

    file = fopen(filename, "w");
    if (!file) {
        perror("fopen");
        fclose(temp);
        exit(EXIT_FAILURE);
    }

    rewind(temp);
    while (fgets(line, sizeof(line), temp)) {
        fputs(line, file);
    }

    fclose(file);
    fclose(temp);
}


// Mark a key as used
void mark_key_as_used(const char *filename, const char *key) {
    printf("mark_key_as_used %s\n",filename);
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    FILE *temp = tmpfile();
    if (!temp) {
        perror("tmpfile");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char line[MAX_KEY_LEN];
    int found = 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline

        // Check if the line matches the exact key or already has the "Used_" prefix
        if (strcmp(line, key) == 0) {
            fprintf(temp, "Used_%s\n", line);
            found = 1;
        } else if (strncmp(line, "Used_", 5) == 0 && strcmp(line + 5, key) == 0) {
            // Key is already marked as used; copy it as-is
            fprintf(temp, "%s\n", line);
            found = 1;
        } else {
            fprintf(temp, "%s\n", line);
        }
    }

    fclose(file);

    if (!found) {
        fprintf(stderr, "Key '%s' not found in %s.\n", key, filename);
    }

    file = fopen(filename, "w");
    if (!file) {
        perror("fopen");
        fclose(temp);
        exit(EXIT_FAILURE);
    }

    rewind(temp);
    while (fgets(line, sizeof(line), temp)) {
        fputs(line, file);
    }

    fclose(file);
    fclose(temp);
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s -g <length> | -d <key> | -u <key>\n", argv[0]);
        return EXIT_FAILURE;
    }

    srand(time(NULL));

    if (strcmp(argv[1], "-g") == 0) {
        int len = atoi(argv[2]);
        char *key = (char *)gen_key(len);
        printf("Generated key: %s\n", key);
        append_to_file(KEYS_FILE, key);
        append_to_file(USED_KEYS_FILE, key);
        free(key);
    } else if (strcmp(argv[1], "-d") == 0) {
        const char *key = argv[2];
        delete_key_from_file(KEYS_FILE, key,0);
        delete_key_from_file(USED_KEYS_FILE, key,1);
    } else if (strcmp(argv[1], "-u") == 0) {
        const char *key = argv[2];
        mark_key_as_used(USED_KEYS_FILE, key);
    } else {
        fprintf(stderr, "Invalid option: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
