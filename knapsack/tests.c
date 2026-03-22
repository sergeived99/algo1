#include "knapsack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

static void print_usage(const char *prog) {
    printf("Usage: %s [--demo]\n", prog);
    printf("  --demo    Запустить демонстрацию работы алгоритма\n");
}

static void demo(void) {
    ItemList *list = createItemList(2);
    if (!list) {
        fprintf(stderr, "Ошибка создания списка\n");
        return;
    }

    addItem(list, 3, 1500, "Ноутбук");
    addItem(list, 1, 2000, "Камера");
    addItem(list, 15, 300, "Книги");
    addItem(list, 20, 200, "Гантели");
    addItem(list, 10, 500, "Палатка");
    addItem(list, 5, 400, "Спальник");

    int capacity = 50;

    printf("=== Исходный список предметов ===\n");
    printItemList(list);
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
    int opt = knapsack01Optimized(list, capacity);
    if (opt >= 0)
        printf("Максимальная стоимость: %d\n", opt);
    else
        printf("Ошибка при вычислении knapsack01Optimized\n");

    freeItemList(list);
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    if (argc == 2 && strcmp(argv[1], "--demo") == 0) {
        demo();
    } else {
        print_usage(argv[0]);
    }
    return 0;
}
