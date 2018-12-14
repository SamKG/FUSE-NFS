#include "hashtable.h"

void hashtable_init(hashtable* ht){
	printf("Initializing hashtable\n");
	ht->size = 20;
	ht->tbl = (node**)malloc(sizeof(node)*ht->size);
	for (int i= 0 ; i <ht->size ; i++){
		ht->tbl[i] = NULL;
	}
}

int hashfunc(hashtable* ht, const char* key){
	int hashval = 1;
	for (int i = 0 ; i < strlen(key) ; i++){
		hashval = hashval*3 + key[i];	
	}
	hashval = hashval % (ht->size);
	if (hashval < 0){
		hashval *= -1;
		hashval = hashval % (ht->size);
	}
	printf("Calculated hashval %d for %s\n",hashval,key);
	return hashval;	
}

int ht_lookup(hashtable* ht, const char* key){
	int hashval = hashfunc(ht,key);
	node* curr = ht->tbl[hashval];
	while(curr != NULL){
		if (strcmp(curr->key,key) ==0){
			printf("Comparing %s to %s\n",curr->key,key);
			return curr->value;
		}	
		curr = curr->next;
	}
	return -1;
}
void ht_delete(hashtable* ht, const char* key){
	int hashval = hashfunc(ht,key);
	node* prev = NULL;
	node* curr = ht->tbl[hashval];
	while(curr != NULL){
		printf("HTDEL COMPARING %s to %s\n",key,curr->key);
		if (strcmp(curr->key,key) ==0){
			if (prev == NULL){
				ht->tbl[hashval] = curr->next;
			}
			else {
				prev->next = curr->next;
			}
			return;
		}	
		prev = curr;
		curr = curr->next;
	}
	return ;
}
int ht_ins(hashtable* ht, const char* key, int value){
	int hashval = hashfunc(ht,key);		
	node* curr = ht->tbl[hashval];
	while(curr != NULL){
		if (strcmp(curr->key,key) ==0){
			return curr->value;
		}	
		curr = curr->next;
	}
	node* newnode = (node*) malloc(sizeof(node));
	newnode->next = ht->tbl[hashval];
	newnode->key = key;
	newnode->value = value;
	ht->tbl[hashval] = newnode;
	printf("INSERTED INTO HT! %s\n",ht->tbl[hashval]->key);
	return value;
}

