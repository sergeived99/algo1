#include "knapsack.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    ItemList *list = createItemList(2);
    if (!list) {
        fprintf(stderr, "Ошибка создания списка\n");
        return 1;
    }

    addItem(list, 3, 1500, "Ноутбук");
    addItem(list, 1, 2000, "Камера");
    addItem(list, 15, 300, "Книги");
    addItem(list, 20, 200, "Гантели");
    addItem(list, 10, 500, "Палатка");
    addItem(list, 5, 400, "Спальник");

    printf("=== Исходный список предметов ===\n");
    printItemList(list);

    int capacity = 50;
    printf("\nВместимость рюкзака: %d кг\n", capacity);

    printf("\n=== Решение методом табуляции (полная таблица) ===\n");
    KnapsackResult *res = knapsack01(list, capacity);
    if (res) {
        printKnapsackResult(res, list);
        freeKnapsackResult(res);
    } else {
        printf("Ошибка при вычислении knapsack01\n");
    }

    printf("\n=== Оптимизированная версия (память O(W)) ===\n");
    int optValue = knapsack01Optimized(list, capacity);
    if (optValue >= 0)
        printf("Максимальная стоимость: %d\n", optValue);
    else
        printf("Ошибка при вычислении knapsack01Optimized\n");

    freeItemList(list);
    return 0;
}
