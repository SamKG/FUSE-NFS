#include <SNFS.h>
typedef struct node{
	struct node* next;
	const char* key;
	int value;
} node;
typedef struct hashtable{
	node** tbl;
	int size;
} hashtable;

void hashtable_init(hashtable* ht);

int hashfunc(hashtable* ht, const char* key);

int ht_lookup(hashtable* ht, const char* key);

void ht_delete(hashtable* ht, const char* key);

int ht_ins(hashtable* ht, const char* key, int value);
