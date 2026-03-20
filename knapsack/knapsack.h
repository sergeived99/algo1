#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <stddef.h>
typedef struct {
    int weight;
    int value;
    char name[64];
} Item;
typedef struct {
    Item *items;
    int size;
    int capacity;
} ItemList;
typedef struct {
    int totalValue;
    int totalWeight;
    int *taken;      // taken[i] = 1, если i-й предмет взят
    int takenCount;
} KnapsackResult;
ItemList *createItemList(int capacity);

int addItem(ItemList *list, int weight, int value, const char *name);

void freeItemList(ItemList *list);

void printItemList(ItemList *list);

KnapsackResult *knapsack01(ItemList *items, int capacity);

int knapsack01Optimized(ItemList *items, int capacity);

void printDPTable(int **dp, int n, int capacity, ItemList *items);

void reconstructSolution(int **dp, ItemList *items, int capacity, int *taken);

void printKnapsackResult(KnapsackResult *result, ItemList *items);

void freeKnapsackResult(KnapsackResult *result);

#endif // KNAPSACK_H
