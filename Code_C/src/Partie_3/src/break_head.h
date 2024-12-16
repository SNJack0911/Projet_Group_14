#ifndef BREAK_HEAD_H
#define BREAK_HEAD_H

#define ALPHABET_SIZE 26
#define MAX_CARACTERES 62  
#define MAX_LIGNS 100       // Nombre maximum de lignes à lire par batch
#define MAX_LEN_LIGNS 40    // Longueur maximum d'une ligne
#define MAX_SIZE_WORDTAB 100
#define MAX_LONGUEUR_MOT 40
#define MAX_KEYS_COUNT 1000
typedef struct {
    char *key;
    double distance;
} KeyDistance;

//Code pour C1
void print_list_clef();
void clefAdmissible(char *chiffre, char **clef, int key_length) ;
void cartesianProductRecursive(char **sets, int *sizes, char *result, int index, int key_length,FILE*file);
void exec_c1(const char *ciphertext, int key_length);
//Code pour C2
char *read_file(const char *filename);
void calculate_frequencies(const char *text, double frequencies[ALPHABET_SIZE]);
void decrypt_xor(const char *ciphertext, char *plaintext, const char *key);
double calculate_distance(const double freq1[ALPHABET_SIZE], const double freq2[ALPHABET_SIZE]);
int compare_key_distance(const void *a, const void *b);
void exec_c2(const char *ciphertext, const char *language);
//Code pour C3
int search_in_dictionary(const char *word, const char *dictionary);
char ** text_to_tab(const char * text, int * word_count);
char **read_keys(const char *file_path,int* keys_count);
char* break_code_c3(const char **keys_tab, int keys_count,const char *dictionary, const char *ciphertext_file_path);
void exec_c3(const char *ciphertext,const char *dictionary);

#endif