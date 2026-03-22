#include "knapsack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ItemList *createItemList(int capacity) {
    ItemList *list = (ItemList*)malloc(sizeof(ItemList));
    if (!list) return NULL;

    list->items = (Item*)malloc(capacity * sizeof(Item));
    if (!list->items) {
        free(list);
        return NULL;
    }

    list->size = 0;
    list->capacity = capacity;
    return list;
}

int addItem(ItemList *list, int weight, int value, const char *name) {
    if (!list) return -1;

    if (list->size >= list->capacity) {
        int new_cap = list->capacity * 2;
        Item *new_items = (Item*)realloc(list->items, new_cap * sizeof(Item));
        if (!new_items) return -1;
        list->items = new_items;
        list->capacity = new_cap;
    }

    list->items[list->size].weight = weight;
    list->items[list->size].value = value;
    strncpy(list->items[list->size].name, name, 63);
    list->items[list->size].name[63] = '\0';
    list->size++;
    return 0;
}

void freeItemList(ItemList *list) {
    if (list) {
        free(list->items);
        free(list);
    }
}

void printItemList(ItemList *list) {
    if (!list) {
        printf("ItemList is NULL\n");
        return;
    }
    printf("Item list (size = %d, capacity = %d):\n", list->size, list->capacity);
    for (int i = 0; i < list->size; ++i) {
        printf("  [%d] %s: вес = %d, стоимость = %d\n",
               i, list->items[i].name, list->items[i].weight, list->items[i].value);
    }
}

static int **allocateDPTable(int rows, int cols) {
    int **dp = (int**)malloc(rows * sizeof(int*));
    if (!dp) return NULL;

    for (int i = 0; i < rows; ++i) {
        dp[i] = (int*)malloc(cols * sizeof(int));
        if (!dp[i]) {
            for (int j = 0; j < i; ++j) free(dp[j]);
            free(dp);
            return NULL;
        }
    }
    return dp;
}

static void freeDPTable(int **dp, int rows) {
    if (!dp) return;
    for (int i = 0; i < rows; ++i) free(dp[i]);
    free(dp);
}


void printDPTable(int **dp, int n, int capacity, ItemList *items) {
    (void)items; // подавление предупреждения
    printf("\nТаблица DP (i\\w):\n");
    printf("   ");
    for (int w = 0; w <= capacity; ++w) {
        printf("%4d", w);
    }
    printf("\n");
    for (int i = 0; i <= n; ++i) {
        printf("%2d ", i);
        for (int w = 0; w <= capacity; ++w) {
            printf("%4d", dp[i][w]);
        }
        printf("\n");
    }
}

void reconstructSolution(int **dp, ItemList *items, int capacity, int *taken) {
    int n = items->size;
    int w = capacity;
    for (int i = n; i >= 1; --i) {
        if (dp[i][w] != dp[i-1][w]) {
            taken[i-1] = 1;
            w -= items->items[i-1].weight;
        } else {
            taken[i-1] = 0;
        }
    }
}

KnapsackResult *knapsack01(ItemList *items, int capacity) {
    int n = items->size;
    int **dp = allocateDPTable(n + 1, capacity + 1);
    if (!dp) return NULL;

    // База
    for (int w = 0; w <= capacity; ++w) dp[0][w] = 0;

    // Заполнение таблицы
    for (int i = 1; i <= n; ++i) {
        int wi = items->items[i-1].weight;
        int vi = items->items[i-1].value;
        for (int w = 0; w <= capacity; ++w) {
            dp[i][w] = dp[i-1][w];  // не берём
            if (w >= wi) {
                int candidate = dp[i-1][w - wi] + vi;
                if (candidate > dp[i][w]) {
                    dp[i][w] = candidate;
                }
            }
        }
    }

    KnapsackResult *res = (KnapsackResult*)malloc(sizeof(KnapsackResult));
    if (!res) {
        freeDPTable(dp, n + 1);
        return NULL;
    }

    res->taken = (int*)malloc(n * sizeof(int));
    if (!res->taken) {
        free(res);
        freeDPTable(dp, n + 1);
        return NULL;
    }

    reconstructSolution(dp, items, capacity, res->taken);

    int totalWeight = 0;
    int takenCount = 0;
    for (int i = 0; i < n; ++i) {
        if (res->taken[i]) {
            totalWeight += items->items[i].weight;
            ++takenCount;
        }
    }
    res->totalValue = dp[n][capacity];
    res->totalWeight = totalWeight;
    res->takenCount = takenCount;

    freeDPTable(dp, n + 1);
    return res;
}

int knapsack01Optimized(ItemList *items, int capacity) {
    int n = items->size;
    int *dp = (int*)calloc(capacity + 1, sizeof(int));
    if (!dp) return -1;

    for (int i = 0; i < n; ++i) {
        int wi = items->items[i].weight;
        int vi = items->items[i].value;
        for (int w = capacity; w >= wi; --w) {
            int candidate = dp[w - wi] + vi;
            if (candidate > dp[w]) {
                dp[w] = candidate;
            }
        }
    }

    int result = dp[capacity];
    free(dp);
    return result;
}

void printKnapsackResult(KnapsackResult *result, ItemList *items) {
    if (!result || !items) {
        printf("Result is NULL\n");
        return;
    }
    printf("Результат:\n");
    printf("  Максимальная стоимость: %d\n", result->totalValue);
    printf("  Общий вес: %d\n", result->totalWeight);
    printf("  Взятые предметы (%d):\n", result->takenCount);
    for (int i = 0; i < items->size; ++i) {
        if (result->taken[i]) {
            printf("    %s (вес %d, стоимость %d)\n",
                   items->items[i].name, items->items[i].weight, items->items[i].value);
        }
    }
}
void freeKnapsackResult(KnapsackResult *result) {
    if (result) {
        free(result->taken);
        free(result);
    }
}
