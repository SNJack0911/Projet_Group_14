#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include "break_head.h"

// Function to test the xor_crypted encryption and decryption
void exec_c1(const char *msg,int key_length) {
  printf("Début de C1...\n");
  FILE *print_file = fopen("/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/src/print_cle_file.txt","w");
  if (!print_file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
    }
  int nbre=0;
  int sizes[key_length];
  char result[key_length];
  char **clef = malloc(key_length * sizeof(char *));
  for (int i = 0; i < key_length; i++) {
    clef[i] = malloc(62);
  }
  clefAdmissible((char*)msg,clef,key_length);
  for (int i = 0; i < key_length; i++) {
    nbre = 0;
    fprintf(print_file,"[");
    for (int k = 0; k < 63 && clef[i][k] != '\0'; k++) {
        nbre++;
        fprintf (print_file,"%c,",clef[i][k]);
    }
    fprintf(print_file,"]\n");
    sizes[i] = nbre;
  }
  FILE *file = fopen("/home/hiraichi/work/Projet_Group_14/Code_C/src/Partie_3/best_key_c1.txt", "w");
  cartesianProductRecursive(clef,sizes,result,0,key_length,file);
  fclose(file);
  fclose(print_file);
  for (int i = 0; i < key_length; i++) {
    free(clef[i]);   // free each row (i.e., each char array)
  }
  free(clef);   // free the array of char* pointers
  printf("Fin de C1\n");
}
