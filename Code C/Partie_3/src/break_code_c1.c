#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#define TAILLE_CLEF 4                               // Longueur de la clé
#define MAX_CARACTERES 62                           // Nombre de caractères dans l'ensemble des clés (lettres + chiffres)

char **clef;                                       // Tableau pour stocker les clés admissibles
int sizes[TAILLE_CLEF];                            // Tailles des ensembles de clés admissibles
char result[TAILLE_CLEF];                          // Mémoire tampon pour les combinaisons des clés

// Fonction pour vérifier si un caractère est admissible
int isAdmissibleChar(int c) {
  return isalnum(c) || isspace(c) || ispunct(c);
}

// Fonction pour générer les caractères admissibles pour chaque position dans la clé
void clefAdmissible(char *chiffre, char **clef) {
    char clefCharacters[MAX_CARACTERES] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int tailleChiffre = strlen(chiffre);

    for (int i = 0; i < TAILLE_CLEF; i++) {
        int m = 0;
        for (int k = 0; k < MAX_CARACTERES; k++) {
            int valid = 1;  // Cờ kiểm tra ký tự hợp lệ qua các vòng lặp
            for (int l = 0; valid && (i + TAILLE_CLEF * l) < tailleChiffre; l++) {
                if (!isAdmissibleChar(clefCharacters[k] ^ chiffre[i + TAILLE_CLEF * l])) {
                    valid = 0; // Si un caractère est invalide, on marque valid à 0 pour sortir de la boucle
                }
            }
            if (valid) {
                clef[i][m++] = clefCharacters[k]; // Ajouter le caractère valide à clef[i]
            }
        }
    }
}


// Fonction pour générer toutes les combinaisons cartésiennes des ensembles admissibles
void cartesianProductRecursive(char **sets, int *sizes, char *result, int index, int n,FILE*file) {
    if (index == n) {
        fwrite(result,sizeof(char),TAILLE_CLEF,file);
        fputs("\n",file);
        return;
    }

    for (int i = 0; i < sizes[index]; i++) {
        result[index] = sets[index][i];
        cartesianProductRecursive(sets, sizes, result, index + 1, n,file);
    }
}


// Function to test the xor_crypted encryption and decryption
void test_xor_crypted(const char *msg) {
    int msg_length = strlen(msg);
    char crypte[msg_length + 1];
    char decrypte[msg_length + 1];
    int nbre=0;

    // Encrypt the message
    printf("Message init : %s\n", msg);
    xor_crypted(msg, crypte,msg_length,"Vin0");
    printf("Message crypte :");

    // Print encrypted message in hexadecimal to make it readable
    for (size_t i = 0; i < strlen(msg); i++) {
        printf("%02x", (unsigned char)crypte[i]);
    }
    printf("\n");
    // Decrypt the message
    xor_crypted(crypte, decrypte,msg_length,"Vin0");
    printf("Message decrypte: %s\n", decrypte);

    // Verify if the decryption matches the original message
    if (strcmp(msg, decrypte) == 0) {
        printf("Mask Test Valide: Decryption est le meme que message init.\n");
    } else {
        printf("Mask Test Non-valide: Decryption mauvais\n");
    }

    clef = (char**)malloc(TAILLE_CLEF*(sizeof(char*)));
    for (int i=0; i<TAILLE_CLEF;i++){
      clef[i] = (char*) malloc(MAX_CARACTERES);
    }
    clefAdmissible(crypte,clef);
    for (int i=0; i<TAILLE_CLEF;i++){
      nbre=0;
      printf("clef[%d] : [ ",i);
      for (int k=0;k<MAX_CARACTERES;k++){
        if (clef[i][k] != '\0'){
          printf("'%c' ",clef[i][k]);
          nbre++;
        }
      }
      sizes[i]=nbre;
      printf("]\n");
    }
    FILE *file = fopen("listclefsadmissible.txt", "w");
    cartesianProductRecursive(clef,sizes,result,0,TAILLE_CLEF,file);
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2){
      printf("Usage: %s -Ciphertext\n",argv[0]);
      exit(1);
    }

    FILE *fd;
    ssize_t bytesRead;
    char msg[1024];

    if ((fd=fopen(argv[1],"r")) == NULL){
      printf("Echec ouvert fichier\n");
      exit(EXIT_FAILURE);
    }

    if (fgets(msg, sizeof(msg), fd) == NULL) {
        printf("Echec de lecture du fichier\n");
        fclose(fd);
        exit(EXIT_FAILURE);
    }
    test_xor_crypted(msg);
    fclose(fd);
    return 0;
}
