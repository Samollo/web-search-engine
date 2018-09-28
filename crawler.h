#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <python2.7/Python.h>

struct string {
	char *ptr;
	size_t len;
};

PyObject* callParser(char* method, char* argument);
void init_string(struct string *s);
size_t do_write_data(void *ptr, size_t size, size_t nmemb, struct string *s);
char* downloadURL(char *url);
char** getUrls(char *path, int* taille);
int writeIndex(char* path, char* url);
int isVisited(char* url);
int addToVisited(char* url);
void network_graph(FILE * fd, char * url, char ** fils, int * taille);
int launch(FILE * fd, char* url, int profondeur);