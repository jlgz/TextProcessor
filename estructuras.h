/**
 *
 * Linked-list header 
 * 
 * Include this file in order to be able to call the 
 * functions available in linked-list.c. We include
 * here only those information we want to make visible
 * to other files.
 *
 *
 */

/**
 * 
 * The TYPE_LIST_KEY is used to define the type of the primary
 * key used to index data in the list. 
 *
 */

#define TYPE_LIST_KEY char
#define SIZE    3333  //numero de listas enlazadas...
/**
 *
 * This structure holds the information to be stored at each list item.  Change
 * this structure according to your needs.  In order to make this library work,
 * you also need to adapt the functions compEQ and freeListData. For the
 * current implementation the "key" member is used search within the list. 
 *
 */

typedef struct ListData_ {
  // The variable used to index the list has to be called "key".
  TYPE_LIST_KEY *key;

  // This is the additional information that will be stored
  // within the structure. This additional information is associated
  // to the key.
  int numTimes;
} ListData;


/**
 * 
 * The item structure
 *
 */

typedef struct ListItem_ {
  ListData *data;
  struct ListItem_ *next;
} ListItem;

/**
 * 
 * The list structure
 *
 */

typedef struct List_ {
  int numItems;
  ListItem *first;
} List;
/**
*tabla hash, lista de listas enlazadas
*/
typedef struct HashTable_ {
  int cont1;//cont1,cont2  item actual al vaciar la hash
  int cont2;
  List *lists;
} HashTable;
/**
 *
 * Function heders we want to make visible so that they
 * can be called from any other file.
 *
 */

int cont3;
int hash(char *key); 
void insertHash(char *key, HashTable *ht); 
List *getLinkedList(int hash, HashTable *ht);
ListData *getNextItem(HashTable *ht);
void initHashT(HashTable *ht);
void deleteHash(HashTable *ht);
void initList(List *l);
void insertList(List *l, ListData *data);
ListData *findList(List *l, TYPE_LIST_KEY *key);
void deleteFirstList(List *l);
void deleteList(List *l);
void dumpList(List *l);
void numItems(HashTable *ht);
