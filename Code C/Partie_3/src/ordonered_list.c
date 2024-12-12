#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordonered_list.h"

// Fonction pour créer un nouveau nœud
Node* create_node(const char* key, int score) {
    Node* newNode = (Node*)malloc(sizeof(Node)); // Allocation mémoire
    if (newNode == NULL) {
        printf("Erreur : allocation mémoire échouée.\n");
        exit(1);
    }
    strncpy(newNode->elem.key, key, sizeof(newNode->elem.key) - 1); // Copier la clé en toute sécurité
    newNode->elem.key[sizeof(newNode->elem.key) - 1] = '\0';        // Assurer la terminaison de la chaîne
    newNode->elem.score = score;   // Initialiser le score
    newNode->next = NULL;          // Initialiser le pointeur suivant à NULL
    return newNode;
}

// Fonction pour insérer un nœud dans une liste triée (par score décroissant)
Node* add_node(Node* head, const char* key, int score) {
    Node* newNode = create_node(key, score);

    // Cas où la liste est vide ou le nouveau nœud doit être en tête
    if (head == NULL || score > head->elem.score) {
        newNode->next = head;
        return newNode;
    }

    // Parcourir la liste pour trouver la position d'insertion
    Node* temp = head;
    while (temp->next != NULL && temp->next->elem.score >= score) {
        temp = temp->next;
    }

    // Insérer le nouveau nœud à la bonne position
    newNode->next = temp->next;
    temp->next = newNode;

    return head;
}

// Fonction pour afficher la liste
void print_list(Node* head) {
    Node* temp = head;
    while (temp != NULL) {
        printf("Key: %s, Score: %d\n", temp->elem.key, temp->elem.score);
        temp = temp->next;
    }
}

// Libérer la mémoire de la liste
void free_list(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
