/*
Darel Gomez
COP3502
April 21, 2024
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXSTLEN 24 
#define TABLESIZE 300007 

int cash = 100000; // cash will never go below 0 or go past max int value
int complexity = 0;
 
typedef struct item { 
    char name[MAXSTLEN+1]; 
    int qty; //for quantity 
    int saleprice; 
} item; 
 
typedef struct chainNode { 
    item* itemPtr; 
    struct chainNode * next; 
} chainNode; 
 
typedef struct hashTable { 
    chainNode** lists; 
    int size; 
} hashTable;

int hashfunction(char* word, int size) { 
    int len = strlen(word); 
    int res = 0; 
    for (int i=0; i<len; i++) 
        res = (1151*res + (word[i]-'a'))%size; 
    return res; 
}

chainNode* init_Node(char* name, int qty);
item* init_Item(char* name, int qty);

// item - string with no more than 24 characters
// quantity - int of how many items being purchased
// total_price - int of the total purchasing price (in dollars)
void buy(hashTable* inv, char* item, int quantity, int total_price);

// item - string with no more than 24 characters (GUARNATEED that item is in invetory)
// quantity - int of how many items being sold (less than or equal to 1000)
void sell(hashTable* inv, char* item, int quantity);

// item - string with no more than 24 characters (GUARANTEED that item is in invetory)
// new_price - updated price of the time
void change_price(hashTable* inv, char* item, int new_price);

void freeTable(hashTable* inv);
void freeList(chainNode* list);

int main() {
    int n;
    scanf("%d", &n);

    hashTable* inv = (hashTable*) malloc(sizeof(hashTable)); // Inventory
    inv->lists = (chainNode**) calloc(sizeof(chainNode*), TABLESIZE);
    inv->size = TABLESIZE;

    char cmd[MAXSTLEN+1];
    char item[MAXSTLEN+1];
    int quantity, price;
    for(int i = 0; i < n; i++) {
        scanf("%s", cmd);
        if(!strcmp(cmd, "buy")) {
            scanf("%s %d %d", item, &quantity, &price);
            buy(inv, item, quantity, price);
        }
        else if(!strcmp(cmd, "sell")) {
            scanf("%s %d", item, &quantity);
            sell(inv, item, quantity);
        }
        else if(!strcmp(cmd, "change_price")) {
            scanf("%s %d", item, &price);
            change_price(inv, item, price);
        }
        else {
            printf("Invalid command.\n");
            i--;
        }
    }
    printf("%d\n", cash);
    printf("%d\n", complexity);

    freeTable(inv);

    return 0;
}

chainNode* init_Node(char* name, int qty) {
    chainNode* temp = (chainNode*) malloc(sizeof(chainNode));
    temp->itemPtr = init_Item(name, qty);
    temp->next = NULL;

    return temp;
}
item* init_Item(char* name, int qty) {
    item* temp = (item*) malloc(sizeof(item));
    strcpy(temp->name, name);
    temp->qty = qty;
    temp->saleprice = 0;

    return temp;
}

void buy(hashTable* inv, char* item, int quantity, int total_price) {
    cash -= total_price;

    int index = hashfunction(item, inv->size);
    chainNode* node = inv->lists[index];
    int res;
    if(node == NULL) { // This only happens if there are no items stored at this index in the hash table
        inv->lists[index] = init_Node(item, quantity);
        printf("%s %d %d\n", item, inv->lists[index]->itemPtr->qty, cash);
        complexity += 1; 
        return;
    }
    int k = 0;
    while(node != NULL) {
        k += 1;
        res = strcmp(item, node->itemPtr->name);
        if(!res) { // Item was found
            node->itemPtr->qty += quantity;
            printf("%s %d %d\n", item, node->itemPtr->qty, cash);
            complexity += k;
            return;
        }
        else {
            node = node->next;
        }
    }
    // This only happens if there are items stored at this index in the hast table, but the item we're searching for isn't there
    node = init_Node(item, quantity);
    node->next = inv->lists[index];
    inv->lists[index] = node;
    printf("%s %d %d\n", item, node->itemPtr->qty, cash);
    complexity += k+1;
}

void sell(hashTable* inv, char* item, int quantity) {
    int index = hashfunction(item, inv->size);
    chainNode* node = inv->lists[index];
    int res;
    int k = 0;
    while(node != NULL) {
        k += 1;
        res = strcmp(item, node->itemPtr->name);
        if(!res) { // Item was found
            if(quantity <= node->itemPtr->qty) {
                node->itemPtr->qty -= quantity;
                cash += (node->itemPtr->saleprice)*quantity;
                printf("%s %d %d\n", item, node->itemPtr->qty, cash);
            }
            else {
                cash += (node->itemPtr->qty)*(node->itemPtr->saleprice);
                node->itemPtr->qty = 0;
                printf("%s %d %d\n", item, node->itemPtr->qty, cash);
            }
            complexity += k;
            return;
        }
        else {
            node = node->next;
        }
    }
}

void change_price(hashTable* inv, char* item, int new_price) {
    int index = hashfunction(item, inv->size);
    chainNode* node = inv->lists[index];
    int res;
    int k = 0;
    while(node != NULL) {
        k += 1;
        res = strcmp(item, node->itemPtr->name);
        if(!res) { // Item was found
            node->itemPtr->saleprice = new_price;
            complexity += k;
            return;
        }
        else {
            node = node->next;
        }
    }
    printf("Item not found!\n");
    exit(-1);
}

void freeTable(hashTable* inv) {
    for(int i = 0; i < TABLESIZE; i++) {
        if(inv->lists[i] != NULL) freeList(inv->lists[i]);
    }
    free(inv->lists);
    free(inv);
}
void freeList(chainNode* list) {
    if(list->next != NULL) freeList(list->next);
    free(list->itemPtr);
    free(list);
}