#include "crawler.h"

int increm = 0;

PyObject* callParser(char* method, char* argument){
	PyObject *pythonFile, *retour, *strObject, *mod, *fct, *args, *py_args_tuple;
	Py_Initialize();
	PyThreadState *state = Py_NewInterpreter();
	PyRun_SimpleString("import sys; sys.path.insert(0,'.')\n");
	if( (pythonFile = PyString_FromString("parser")) == NULL){
		printf("Echec: Récupération du fichier.py\n");
		PyErr_Print();
	}
	if( (mod = PyImport_Import(pythonFile)) == NULL){
		printf("Echec: Import du module \n");
		PyErr_Print();
	}
	if( (fct = PyObject_GetAttrString(mod, method)) == NULL){
		printf("Echec: Récupération de la méthode \n");
		PyErr_Print();
	}
	if( (args = Py_BuildValue("zz", argument, "") ) == NULL){
		printf("Echec de BuildValue.\n");
		PyErr_Print();
	}
	if( (retour = PyEval_CallObject(fct, args)) == NULL){
		printf("Echec: Appel de fonction \n");
		PyErr_Print();
	}
	return retour;
}

void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

size_t do_write_data(void *ptr, size_t size, size_t nmemb, struct string *s){
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

char* downloadURL(char *url){
	CURL *curl_handle;
	char *pagefilename = NULL;
	asprintf(&pagefilename, "./HTML/page%d.html", increm);
	increm = increm+1;
	FILE *pagefile;

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
	// curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	//curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L); 
	curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curl_handle, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | CURLPROTO_FILE);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, fwrite);

	/* open the file */ 
	pagefile = fopen(pagefilename, "w");
	if(pagefile) {

		/* write the page body to this file handle */ 
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

		/* get it! */ 
		curl_easy_perform(curl_handle);
		/* close the header file */ 
		fclose(pagefile);
	}

	/* cleanup curl stuff */ 
	curl_easy_cleanup(curl_handle);
	return pagefilename;
}

char** getUrls(char *path, int* taille){
	PyObject *retour, *strObject;
	if( (retour = callParser("geturls", path) ) == NULL){
		printf("callParser de geturls a échoué\n");
	}
	*taille = PyList_Size(retour);
	if(*taille == 0){return NULL;}
	char** tableau = (char**)malloc(sizeof(char*) * *taille);
	int i;
	char *res;
	for( i = 0; i<*taille; i++){
		strObject = PyList_GetItem(retour, i); 
		res = (char*)PyString_AsString(strObject);
		tableau[i] = res;
	}
	return tableau;
}

int writeIndex(char* path, char* url){
	int i, j; 
	FILE* motfile;
	PyObject *retour, *strObject;

	if( (retour = callParser("extractWord", path) ) == NULL){
		printf("callParser de extractWord a échoué\n");
	}
	int taille = PyList_Size(retour);
	char** tFrequence = (char**)malloc(sizeof(char*) * taille);
	char *res;
	for( j = 0; j<taille; j++){
		strObject = PyList_GetItem(retour, j); 
		res = (char*)PyString_AsString(strObject);
		tFrequence[j] = res;
	}
	for(i =0; i< taille; i = i+2){
		char chemin[9 + strlen(url)];
		strcpy(chemin, "./index/");
		strcat(chemin, tFrequence[i]);
		motfile = fopen(chemin, "a"); // Les paires sont mots et impairs sont fréquences
		fprintf(motfile, "%s %s \n", tFrequence[i+1], url);
		fclose(motfile);
	}
	free(tFrequence);
	return 0;
}

int isVisited(char* url){
	FILE* history;
	char* ligne = malloc(sizeof(char)*100);
	size_t len = 0;
	ssize_t read;
	int cmp = 0;
	char buff;
	char* token;
	history = fopen("./history/visited.txt", "r");
	if(history == NULL){printf("ERREUR\n");}
	fseek(history, 0, SEEK_SET);
	while((fgets(ligne, sizeof(char)*99, history )) != NULL){
		ligne[strlen(ligne)-2] = '\0';
		if( strcmp(ligne, url) == 0){
			return 1;
		}
	}
	fclose(history);
	free(ligne);
	return 0;
}

int addToVisited(char* url){
	FILE* history;
	history = fopen("./history/visited.txt", "a");
	fprintf(history, "%s \n", url);
	return 0;
}

void network_graph(FILE * fd, char * url, char ** fils, int * taille){
	int i;
	for(i = 0; i<*taille; i++){
		fprintf(fd, "\"%s\" -> \"%s\";\n", url, fils[i] );
	}
}

int launch(FILE * fd, char* url, int profondeur){
	printf("Je visite: %s\n", url);
	printf("Profondeur de %d\n", profondeur);
	if( isVisited(url) == 0){
		addToVisited(url);
		char* path = downloadURL(url); // Je télécharge l'URL dans le répertoire et je récupère son chemin
		char** url_inside;
		int* taille = malloc(sizeof(int));
		if( (url_inside = getUrls(path, taille) ) != NULL){ // Je récupère toutes les URLs visitable dans l'URL courant
			int indice = 0;
			// Si la profondeur le permet, et que je ne suis pas à la fin du tableau, je fais un appel récursif
			while( indice < *taille && profondeur > 0 ){ 
				if(url_inside[indice][0] == '/') printf("test du slash\n");
				launch(fd, url_inside[indice], profondeur - 1);
				indice = indice + 1;
			}
			network_graph(fd, url, url_inside, taille);
		}
		writeIndex(path, url);

	}else if( isVisited(url) == 1){
		printf("L'url a déjà été visitée\n");
	}
	return 0;
}

int main(void){
	FILE * fd;
	fd = fopen("sitemap.dot","a");
	if(fd){
		fprintf(fd, "%s\n", "digraph{");
	} else {
		fprintf(stderr, "Problème d'ouverture :%s\n", "sitemap.dot");
	}
	printf("On démarre le launch\n");
	launch(fd, "http://upsilon.cc/~zack/stuff/cproj/test.html", 4);
	launch(fd, "http://www.lesnumeriques.com/", 4);
	launch(fd, "https://www.univ-paris-diderot.fr", 4);
	launch(fd, "https://objectif-rentier.fr", 4);
	printf("Crawl réussi avec succès !\n");
	printf("La base de données a été créée.\n");
	fprintf(fd, "%s\n","}");
	return 0;
}