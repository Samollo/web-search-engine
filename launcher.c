#include "launcher.h"

void tri_insertion(tuple* tab, int taille)
{
	int i, j;
	for (i = 1; i < taille; ++i) {
		int elem = tab[i].occ;
		char* str = tab[i].url;
		for (j = i; j > 0 && tab[j-1].occ > elem; j--){
			tab[j].occ = tab[j-1].occ;
			tab[j].url = tab[j-1].url;
		}
		tab[j].occ = elem;
		tab[j].url =str ;
	}
}

tuple* rechercheMot(char* mot){
	FILE* motfile;
	char ligne[100];
	size_t len = 0;
	ssize_t read;
	bool res = false;
	char* token;
	char buff;
	int cmp = 0;

	char path[9+ strlen(mot)];
	strcpy(path, "./index/");
	strcat(path, mot);
	motfile = fopen(path, "r");
	if(motfile == NULL){
		return NULL;
	}

	while(!feof(motfile)){ // Je parcours le fichier et compte le nombre de lignes
		buff = fgetc(motfile);
		if(buff == '\n'){cmp++;}
	}
	fseek(motfile, 0, SEEK_SET);
	tuple* liste = malloc(sizeof(tuple)*cmp); // Je déclare mon tableau de tuple
	int i = cmp;

	while( (fgets(ligne, sizeof(char)*99, motfile )) != NULL && i>0){ // Je parcours le fichier et je récupère l'url et son occurence
		token = strtok(ligne, " ");
		liste[cmp - i].cmp = i;
		liste[cmp - i].occ = atoi(token); // Stock occ dans le tuple
		token = strtok(NULL, " ");
		liste[cmp - i].url = malloc(strlen(token));
		strcpy (liste[cmp - i].url, token);// Stock url dans le tuple
		i--;
	}
	fclose(motfile);

	return liste;
}


void viderBuffer()
{
	int c = 0;
	while (c != '\n' && c != EOF)
	{
		c = getchar();
	}
}

int lire(char *chaine, int longueur)
{
	char *positionEntree = NULL;

	if (fgets(chaine, longueur, stdin) != NULL)
	{
		positionEntree = strchr(chaine, '\n');
		if (positionEntree != NULL)
		{
			*positionEntree = '\0';
		}
		else
		{
			viderBuffer();
		}
		return 1;
	}
	else
	{
		viderBuffer();
		return 0;
	}
}



int main(){
	int i;
	char mot[20];
	bzero(mot, 20);
	printf("Bonjour ! Si vous souhaitez quitter le programme, entrez ^QUIT svp. \nEntrez votre recherche (1 mot) : \n");
	lire(mot, 20);
	while(strcmp(mot, "^QUIT") != 0){
		tuple* res;
		if(( res = rechercheMot(mot)) != NULL){
			tri_insertion(res, res[0].cmp);
			int size = res[0].cmp;
			printf("Mot recherché : %s \nRésulats de la recherche (plus forte occurence à plus faible) : \n ", mot);
			for(i = 0; i<size; i++){
				printf("--> %s\n", res[size-1-i].url);
			}
			for(i = 0; i < res[0].cmp; i++){
				free(res[i].url);
			}
			free(res);
		} else{
			printf("Le mot demandé n'est pas présent dans notre base. \n");
		}
		printf("Entrez votre recherche (1 mot) : \n");
		lire(mot, 20);
	}
	printf("Merci !\n");
	return 0;

}