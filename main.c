/**
 *
 * Main file 
 * 
 *
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "red-black-tree.h"
#include "estructuras.h"
#include "pthread.h"
#define MAXCHAR  100
int acabado = 0;
int contf = 0;
int M = 4;
pthread_cond_t condd = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
int vuelta=0;
int dw=0;
int dr=0;
struct circular_buf{
    int size;
    int w;
    int r;
    HashTable **lh;
    int *ind;
}circular_buf;
struct circular_buf *cb;
int compara_floats(const void *p1, const void *p2)
{
	float *num1, *num2;
	num1 = (float *) p1;
	num2 = (float *) p2;
	if (*num1 < *num2)
		return -1;
	if (*num1 > *num2)
		return 1;
	return 0;
}

void toGNU(){
  while ( getchar() != '\n' ) ; 
  char c;
  FILE *fp = popen("gnuplot", "w");
  char *st1 = "set yrange [0:1] \n";
  char *st2 = "plot \"grafica.data\"   with lines\n";
  char *st3 = "quit \n";
  fwrite(st1 , sizeof(char),strlen(st1),fp);
  fwrite(st2 , sizeof(char),strlen(st2),fp);
  fflush(fp);
  printf("press enter to close gnuplot\n ");
  scanf("%c",&c);
  fwrite(st3 , sizeof(char),strlen(st3),fp);
  fflush(fp);
  fclose(fp);
  
}
void probability(RBTree *tree, int numFicheros) {
	FILE *fp;
	char *palabra= malloc(sizeof(char) *100);
	fp = fopen("grafica.data","w");
	RBData *treeData;
	float *vector;
	vector = malloc(sizeof(float) * numFicheros);
	float numTotal = 0;
	int i;
	
	printf("Introduce la palabra que quieres buscar: ");
	scanf("%s", palabra);
	treeData = findNode(tree, palabra);
	if (treeData == NULL) {
	  printf("this word isnt in the tree \n");
	}
	else{
	fprintf(fp,"# x  p(x)   \n");
	for(i=0; i < numFicheros; i++) { 
		vector[i] = (float) treeData->vector[i];
		numTotal = numTotal + vector[i];
	}
	qsort(vector, numFicheros, sizeof(float), compara_floats); /* (vector, numElemVector, sizeof(typOfElemVector), compara_sencers) */
	for(i=0; i < numFicheros; i++) {
		if (numTotal  == 0 ){ vector[i] = 0; }
		else{
		  vector[i] = (float)( vector[i] / numTotal);
		 
		}
		fprintf(fp,"%d  %f \n",i,vector[i]);
	}
	fflush(fp);
	toGNU();
	}
	free(palabra);
	free(vector);
	fclose(fp);
	
	
	
}  


/**
 *
 *pasa a minusculas palabra
 *
 */
void toLowerCase(char *palabra){
	int i = 0;
	while(palabra[i] != '\0'){
		palabra[i] = tolower(palabra[i]);
		i++;
	}
}
/*
 * Recorre libro y las extrae a una tabla hash y vuelca la informacion en un arbol
*/
void processbook(char *filename,int fich)
{
	
	printf("procesando fichero: %s hilo: %d\n",filename,(int)pthread_self());
	int i,cond;
	HashTable *ht;
	ht = malloc(sizeof(HashTable));
	initHashT(ht);
	char *str;
	char *str_tmp;
	FILE *fp1;
	fp1 = fopen(filename, "r");
	str = (char *) malloc(sizeof(char) * MAXCHAR);//linea
	str_tmp = (char *) malloc(sizeof(char) * MAXCHAR);//secuencia
  	if (!fp1) {
    		printf("No he pogut obrir fitxer.%s \n", filename);
   	 	exit(1);
  	}
  	fgets(str, MAXCHAR, fp1);
	int cont = 0;//numero de caracteres de la secuencia
	int valid = 1;//validacion de la secuencia
	//int contcont = 1;
	cond = 0;
  	while (!feof(fp1) || cond) {
  		for(i = 0; i <= strlen(str);i++){
			if (isalpha(str[i]) && valid){
      				str_tmp[cont] = str[i];
				cont++;
			}
   		        else if (isdigit(str[i])){
				valid = 0;
			}
    			else if (ispunct(str[i])){
				if ((str[i] == (char)39  )&& valid){
				  str_tmp[cont] = str[i];
				  cont++;
				}
				// !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
				else if((str[i] ==(char)33) ||(str[i] == (char)63) ||(str[i] == (char)46)||(str[i] == (char)44)||(str[i] ==(char)50)||(str[i] ==(char)51)||(str[i]  == (char)34)||(str[i] ==(char)58)||(str[i] ==(char)59)||(str[i] ==(char)45)||(str[i] ==(char)40)||(str[i] ==(char)41)){
				    if(valid && (cont>0)){
					str_tmp[cont] = '\0';
					toLowerCase(str_tmp );
     			        	insertHash(str_tmp,ht);
					valid = 1;
					cont = 0;
				}
				}
				else{ 
				  valid = 0;
				  cont = 0;    
				}
				
			}
    			else if (isspace(str[i])){
				if(valid && (cont>0)){
					str_tmp[cont] = '\0';
					toLowerCase(str_tmp );
     			        	insertHash(str_tmp,ht);
				}
				cont = 0;
				valid = 1;
			}
    			else {
				valid = 0;
			}
		}
		if(feof(fp1) && cond){
		  break;
		}
     		fgets(str, 100, fp1);
		valid = 1;
		cont = 0;
		//contcont++;
		if(feof(fp1) && !cond){
		  cond = 1;
		}
  	}
	
//recorrido de la hash
	pthread_mutex_lock(&mutex2);
	//printf(" en  w vuelta %d dw %d, dr %d,  w %d, r %d \n",vuelta,dw,dr,cb->w,cb->r);
	while (vuelta == 1 && cb->w>=cb->r){
	    dw = 1;
	//printf(" en wd dw %d, dr %d,  w %d, r %d \n",dw,dr,cb->w,cb->r);
	    pthread_cond_wait(&condd,&mutex2);
	}
	cb->lh[cb->w] = ht;
	cb->ind[cb->w] = fich;
	cb->w +=1;
	if (cb->w == cb->size){
		cb->w = 0;
		vuelta = 1;
	}	
	if(dr == 1 && ((vuelta == 0 && cb->w > cb->r) ||(vuelta == 1 && cb->w < cb->r))){
	pthread_cond_signal(&condd);
	dr = 0;
	}
	contf +=1;
	pthread_mutex_unlock(&mutex2);
	//numItems(ht);
	free(str);
	free(str_tmp);
	fclose(fp1);

}
/**
 * Lee las palabras del fichero filename  y las inserta en un arbol binario
 */
void processDictionary(char *filename,int nFix, RBTree *tree)
{
  int i = 0;
  RBData *treeData;
  
  /* Allocate memory for tree */

  /* Initialize the tree */
  initTree(tree);
  
  FILE *fp;

  char *paraula;

  paraula = (char *) malloc(sizeof(char) * MAXCHAR);

  fp = fopen(filename, "r");
  if (!fp) {
    printf("Could not open file '%s'\n", filename);
    exit(1);
  }

  while (fscanf(fp, "%s", paraula) != EOF)
  {
    toLowerCase(paraula);
    //printf("%s\n", paraula);
    fflush(stdout);
    treeData = findNode(tree, paraula); 

    if (treeData != NULL) {
      
      /* If the key is in the tree increment 'num' */
      treeData->num++;
    } else {

      /* If the key is not in the tree, allocate memory for the data
       * and insert in the tree */

      treeData = malloc(sizeof(RBData));
      treeData->key = malloc(sizeof(char) * MAXCHAR);
      strcpy(treeData->key, paraula); // se copia el contenido del puntero
      
      treeData->num = 1;
      treeData->vector = malloc(sizeof(int) * nFix);
      for(i = 0; i<nFix;i++){
	(treeData->vector)[i] = 0;
      }
      insertNode(tree, treeData);
  }
  }
  printf("Nombre total de nodes a l'arbre: %d\n", getNumNodes(tree));

  fclose(fp);

  free(paraula);
  
}

struct param{
	int i;
	int nfich;
	char **b_t_p;
}param;
void *thread_fun(void *arg){
	int j;
	struct param *par = (struct param*)arg;
	while(par->i < par->nfich){
		pthread_mutex_lock(&mutex);
		par->i +=1;
		j = par->i-1;
		
		pthread_mutex_unlock(&mutex);
		if(j< par->nfich)
		  processbook((par->b_t_p)[j],j);
	}
	if (contf == par->nfich){
	  acabado = 1;}
	return NULL;
}
void *thread_fun_h(void *arg){
	if(acabado && (vuelta ==0) &&(cb->r>=cb->w)){
		//printf("fnish read vuelta %d dw %d, dr %d,  w %d, r %d \n",vuelta,dw,dr,cb->w,cb->r);
		return 0;}
	HashTable *ht;
	int fich;
	RBTree *arbol = (RBTree *) arg;
	pthread_mutex_lock(&mutex2);
	//printf(" en r vuelta %d dw %d, dr %d,  w %d, r %d \n",vuelta,dw,dr,cb->w,cb->r);
	while((vuelta ==0) &&(cb->r>=cb->w)){
		//printf(" en rd dw %d, dr %d,  w %d, r %d \n",dw,dr,cb->w,cb->r);
		dr = 1;
	  	pthread_cond_wait(&condd,&mutex2);
	  }
	  ht = (cb->lh)[cb->r];
	  fich = (cb->ind)[cb->r];
	  cb->r +=1;
	  if(cb->r == cb->size){
		cb->r =0;
		vuelta = 0;}
	  if(dw ==1 && (((vuelta ==1) && (cb->w < cb->r)) ||((vuelta ==0 )&& (cb->r< cb->w)))) {
		dw =0;
		pthread_cond_broadcast(&condd);
	  }
	  pthread_mutex_unlock(&mutex2);
	  ListData *ld = getNextItem(ht);
	  while(ld != NULL){
	  //printf("%s %d \n ",ld->key,ld->numTimes);
	      RBData *f = findNode(arbol,ld->key);
	      if(f != NULL){
		  (f->vector)[fich]= ld->numTimes; 
	    //printf("%s %d %d \n ", ld->key,f->num,f->vector[fich]);
	      }
	      ld = getNextItem(ht);
	  }
	  deleteHash(ht);
	  free(ht);
	  return thread_fun_h(arbol);
	
}
//--tool= helgrind
int create_tree( RBTree *tree){
  deleteTree(tree);
  initTree(tree);
  clock_t t1;
  clock_t t2;
  char *filename1=malloc(sizeof(char)*100);
  char *filename2=malloc(sizeof(char)*100);
  printf("introduce el nombre del diccionario \n");
  scanf("%s",filename1);
  printf("introduce el nombre del fichero de configuracion \n");
  scanf("%s",filename2);
  struct param *par= malloc(sizeof(struct param));
  pthread_t ntid[M+1];
  int i=0;
  char **books_to_process;
  int contador; //numero de ficheros
  char  *str; // direccion del fichero
  str = (char *) malloc(sizeof(char) * MAXCHAR);
  FILE *fp = fopen(filename2, "r");
  //fp = fopen("8celt10.txt", "r");
  fscanf(fp,"%d", &contador);
  books_to_process   = malloc(sizeof(char *)*contador);
  for(i = 0; i< contador; i++) {
      books_to_process[i] = malloc(sizeof(char)*MAXCHAR);
  }
  i = 0;
  fgets(str, MAXCHAR, fp);
  processDictionary(filename1,contador,tree);
  while(i <  contador) {
    fgets(str, MAXCHAR, fp);
    str[strlen(str)-1] = '\0';
    strcpy(books_to_process[i], str); 
    i++;
  } 
  par->i = 0;
  par->nfich = contador;
  par->b_t_p = books_to_process; 
  t1=clock();
  for(i = 0;i<M;i++){
  	pthread_create(&(ntid[i]),NULL,thread_fun,(void *)par);
  }
  i = pthread_create(&(ntid[M]),NULL,thread_fun_h,(void *)tree);
  for(i = 0;i<=M;i++){
  	pthread_join(ntid[i],NULL);
  }
  t2=clock();
  printf("tiempo de procesado de ficheros %f \n",(double)(t2-t1)/(double)CLOCKS_PER_SEC);
  for(i = 0; i< contador; i++) {
      free(books_to_process[i]);
  }
  contf = 0;
  acabado = 0;
  cb->w = 0;
  cb->r = 0;
  dw = 0;
  dr = 0;
  vuelta =0;
  free(filename1);
  free(filename2);
  free(books_to_process);
  free(par);
  fclose(fp);
  free(str);
  return contador;
}



void menu(){
	int opcion = 0;
	int nFich=0;
	RBTree *arbol = malloc(sizeof(RBTree));
	initTree(arbol);
	char *filename3 = "treeBytes";
	while (opcion != 5) {
		printf("1. Creacio de larbre\n");
		printf("2. Emmagatzemament de larbre\n");
		printf("3. Lectura de larbre\n");
		printf("4. Grafica de la probabilitat que surti una determinada paraula\n");
		printf("5. Sortida\n");
		scanf("%d", &opcion);

		switch(opcion) {
			case 1:
				nFich = create_tree(arbol);
				break;
			case 2:
				initDFS_save(filename3,arbol,nFich);
				break;
			case 3:
				if(arbol){
				  deleteTree(arbol);
				  free(arbol);
				  arbol = malloc(sizeof(RBData));
				}
				nFich = load_tree(filename3 ,  arbol);
				break;
			case 4:
				probability(arbol, nFich);
				break;
			case 5:
				break;
			default:
				printf("this option so noob\n");
				break;
		}
		
	}
	deleteTree(arbol);
	free(arbol);

}
/**
 *llena un arbol con las palabras del fichero argv[1]
 * y por cada fichero en argv[2] lo procesa 
 *  
 *
 */

int main()
{
  
  cb = malloc(sizeof(circular_buf));
  cb->w = 0;
  cb->r = 0;
  cb->size = M;
  cb->lh = malloc(sizeof(HashTable*) *M); 
  cb->ind = malloc(sizeof(HashTable*) *M); 
  menu();
  free(cb->lh);
  free(cb->ind);
  free(cb);
  return 0;
  
}
