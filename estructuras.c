/**
 *
 * Linked-list implementation. 
 * 
 * This is an implementation of a simple linked-list. A minimal
 * set of necessary functions have been included.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * We include the linked-list.h header. Note the double
 * quotes.
 */

#include "estructuras.h"

/**
 *
 * Free data element. The user should adapt this function to their needs.  This
 * function is called internally by deleteList.
 *
 */

static void freeListData(ListData *data)
{
  free(data->key);
  free(data); 
}

/**
 *
 * Dumps data contents to stdout. To be used for debugging.
 *
 */

static void dumpListData(ListData *data)
{
  printf("Key %s appears %d times\n", data->key, data->numTimes);
}

/**
 *
 * Compares if key1 is equal to key2. Should return 1 (true) if condition
 * is satisfied, 0 (false) otherwise.
 *
 */

static int compEQ(TYPE_LIST_KEY *key1, TYPE_LIST_KEY *key2)
{
  int rc;

  rc = 0;

  if (strcmp(key1 , key2) == 0 )
    rc = 1;

  return rc;
}

/**
 * 
 * Please do not change the code below unless you really know what you are
 * doing.
 *
 */

/**
 * 
 * Initialize an empty list
 *
 */

void initList(List *l)
{
  l->numItems = 0;
  l->first = NULL;
}

/**
 * 
 * Insert data in the list.  This function does not perform a copy of data
 * when inserting it in the list, it rather creates a list item and makes
 * this item point to the data. Thus, the contents of data should not be
 * overwritten after calling this function.
 *
 */

void insertList(List *l, ListData *data)
{
  ListItem *tmp, *x;

  x = malloc(sizeof(ListItem));

  if (x == 0) {
    printf("insufficient memory (insertItem)\n");
    exit(1);
  }

  /* Insert item at first position */

  tmp = l->first;
  l->first = x;
  x->next = tmp;

  /* Link data to inserted item */
  x->data = data;

  l->numItems++;
}

/**
 * 
 * Find item containing the specified key. Returns the data
 * that it points to (not the item itself).
 *
 */

ListData *findList(List *l, TYPE_LIST_KEY *key)
{
  ListItem *current;

  current = l->first;

  while (current != NULL)
  {
    if (compEQ(current->data->key, key))
      return (current->data);

    current = current->next;
  }

  return (NULL);
}

/**
 * 
 * Deletes the first item of the list. The data to which
 * the deleted item points to also is deleted.
 *
 */

void deleteFirstList(List *l)
{
  ListItem *tmp;

  tmp = l->first;

  if (tmp)
  {
    l->first = tmp->next;
    freeListData(tmp->data);
    free(tmp);
    l->numItems--;
  }
}

/**
 * 
 * Deletes a list including the data to which their 
 * items point to.
 *
 */

void deleteList(List *l)
{
  ListItem *current, *next;

  current = l->first;

  while (current != NULL)
  {
    next = current->next;
    freeListData(current->data);
    free(current);
    current = next;
  }

  l->numItems = 0;
  l->first = NULL;
}

/**
 * 
 * Dumps the contents of the list. Internally this function
 * called dumpListData which is user defined.
 *
 */

void dumpList(List *l)
{
  ListItem *current;

  current = l->first;

  while (current != NULL)
  {
    dumpListData(current->data);
    current = current->next;
  }

  printf("Total number of items: %d\n", l->numItems);
}
/*
*funcion hash usada
*/
int hash( char *key)
{
	unsigned int i, len, seed, sum, hash;
	len = strlen(key) - 1; 
  	sum = 0;
  	seed = 131;
  	for(i = 0; i < len; i++)
   		 sum = sum * seed + (int)key[i];
 	hash = sum % SIZE;
	return hash;
}
/*
*inserta una key a la hash
*/
void insertHash(char *key, HashTable *ht){
	List *list;
    	ListData *listData;
    	int h = hash(key);
    	list = getLinkedList(h, ht);
/* Search if the key is in the tree */
    	listData = findList(list, key); 
	if (listData != NULL) {
/* We increment the number of times current item has appeared */
      		listData->numTimes++;
    	} 
    	else {
/* If the key is not in the list, allocate memory for the data and
       * insert it in the list */
		listData = malloc(sizeof(ListData));
		listData->key = malloc(sizeof(char) * 100);
      		strcpy(listData->key, key);
		//printf("%s %d\n",listData->key,h);
      		listData->numTimes = 1;
		insertList(list, listData);
		cont3++;
		//List *list2 = getLinkedList(h, ht);
		//printf("direc1 %p\n ",&list);
		//printf("direc2 %p\n ",&list2);
		//printf("n %d \n",list->numItems);
		//printf("n %d \n",list2->numItems);
		
		
	}
}
/*
*inicializa la tabla hash
*/
void initHashT(HashTable *ht){
	int i;
	ht->cont1=0;
	ht->cont2=0;
	cont3 = 0;
	ht->lists= malloc(sizeof(List)*SIZE);
	for(i = 0; i<SIZE; i++){
		initList(&((ht->lists)[i]));
	}
}
/*
*libera recursivamente la memoria reservada por la hash
*/
void deleteHash(HashTable *ht){
	int i;
	for(i = 0; i<SIZE; i++){
		deleteList(&((ht->lists)[i]));
	}
	free(ht->lists);
}
/*
*retorna la i-esima lista enlazada
*/
List *getLinkedList(int hash, HashTable *ht){
	return &((ht->lists)[hash]);
}
/**
*funcion diseñada para pruebas
*
*
*/
void numItems(HashTable *ht){
  int i = 0;
  List *l;
  int cont = 0;
  printf("cont3 %d\n ",cont3);
  for(i =0;i<SIZE;i++){
    l = &((ht->lists)[i]);
    cont += l->numItems;
   // printf("n %d \n",l->numItems);
  }
  printf("last cont %d \n",cont);
}
/**
* retorna el siguiente item de la hash.
* para volcar la informacion local...
* next item = hashTable[cont1][cont2]
*/
ListData *getNextItem(HashTable *ht){
	(ht->cont2)++;
	List *l = &((ht->lists)[ht->cont1]);
	int i = 1;
	ListItem *current;
	current = l->first;
	while (i< ht->cont2 && current != NULL)
	{
	  i++;
	  current = current->next;
	  //printf("%s \n",current->data->key);
	}
	while(current == NULL){
	  ht->cont2  = 1;
	  if (ht->cont1 < SIZE - 1){
	    (ht->cont1)++;
	    l = &((ht->lists)[ht->cont1]);
	    current = l->first;
	    //printf("%s \n",current->data->key);
	  }
	  else{
	    return NULL;
	    ht->cont1=0;
	    ht->cont2 = 0;
	  }
	}
	return current->data;
	  
}
