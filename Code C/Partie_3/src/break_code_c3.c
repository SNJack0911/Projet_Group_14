#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fonctions_annexes.h"
#include "ordonered_list.h"

int break_code_c3(const char **keys_tab, int keys_count, const char *dictionary, const char *text) {
    int word_count = 0;
    FILE *decrypt_text;
    int score;
    Node* head = NULL;

    for (int i = 0; i < keys_count; i++) {
        score = 0;
        xor_crypt_decrypt(text, "decrypt_text.txt", keys_tab[i]);

        decrypt_text = fopen("decrypt_text.txt", "r");
        if (decrypt_text == NULL) {
            printf("Error: Can't open file 'decrypt_text.txt'.\n");
            return 1;
        }

        char **word_tab = text_to_tab("decrypt_text.txt", &word_count);
        fclose(decrypt_text);

        for (int j = 0; j < word_count; j++) {
            score += search_in_dictionary(word_tab[j], dictionary);
        }

        head = add_node(head, keys_tab[i], score);
        free_word_tab(word_tab, word_count);
    }

    print_list(head);
    free_list(head);
    return 0;
}

int main(int argc, const char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <dictionary_file> <text_file>\n", argv[0]);
        return 1;
    }

    const char *dictionary = argv[1];
    const char *text = argv[2];

    const char *keys_tab[] = {
        "macle",
        "cle",
        "keys",
        "12345",
        "poele",
        "poule",
        "poulet",
        "chat",
        "rouleau",
        "XM4msgt",
        "bouulette",
        "bheferifnk",
        "jcnhrbevuebvvei145d6v",
        "1e6d6zd1dz15d54ezf87",
        "Zébrée",
        "Japon_né_88",
        "zoommm",
        "pauline",
        "martin",
        "zbi::12mdp58YYZ",
        "Poulet",
        "moulet",
        "poules"
    };
    int keys_count = sizeof(keys_tab) / sizeof(keys_tab[0]);

    break_code_c3(keys_tab, keys_count, dictionary, text);
    return 0;
}
