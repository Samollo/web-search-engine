#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <stdbool.h>


typedef struct tTuple tuple;
struct tTuple  {
    char *url; // or char strVal[20];
    int occ;
    int cmp;
};

void tri_insertion(tuple* tab, int taille);
tuple* rechercheMot(char* mot);
int lire(char *chaine, int longueur);
void viderBuffer();
