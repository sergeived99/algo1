#include "levenshtein.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void testDistance(const char *s1, const char *s2, int expected) {
    int dist = levenshteinDistance(s1, s2);
    printf("levenshteinDistance(\"%s\", \"%s\") = %d", s1, s2, dist);
    if (expected >= 0) {
        printf(" (ожидалось %d) %s", expected, (dist == expected ? "OK" : "FAIL"));
    }
    printf("\n");
}
void testWithOperations(const char *s1, const char *s2) {
    printf("\n--- Преобразование \"%s\" -> \"%s\" ---\n", s1, s2);
    EditResult *res = levenshteinWithOperations(s1, s2);
    if (!res) {
        printf("Ошибка выделения памяти\n");
        return;
    }
    printf("Расстояние: %d\n", res->distance);
    printOperations(res, s1, s2);
    printTransformation(res, s1, s2);
    freeEditResult(res);
}

void demo() {
    printf("=== Демонстрация работы Distance Levenshtein ===\n\n");
    testWithOperations("SATURDAY", "SUNDAY");

    const char *dictionary[] = {"sunday", "saturday", "monday", "tuesday", "wednesday", 
                                 "thursday", "friday", "kitten", "sitting", "cat", "dog"};
    int dictSize = sizeof(dictionary) / sizeof(dictionary[0]);
    char **dict = (char **)dictionary; // для совместимости с API

    const char *word = "sunday";
    int maxDist = 2;
    int count;
    char **similar = findSimilarWords(word, dict, dictSize, maxDist, &count);
    printf("\nСлова, похожие на \"%s\" (расстояние <= %d):\n", word, maxDist);
    if (similar) {
        for (int i = 0; i < count; i++) {
            int dist = levenshteinDistance(word, similar[i]);
            printf("  %s (расстояние %d)\n", similar[i], dist);
        }
        freeSimilarWords(similar, count);
    } else {
        printf("  не найдено\n");
    }
}

void testSimilar() {
    printf("\n=== Тест поиска похожих слов ===\n");
    const char *dictArr[] = {"hello", "hell", "helo", "world", "word", "worm"};
    int dictSize = sizeof(dictArr) / sizeof(dictArr[0]);
    char **dict = (char **)dictArr;

    const char *word = "hell";
    int maxDist = 1;
    int count;
    char **similar = findSimilarWords(word, dict, dictSize, maxDist, &count);
    printf("Слова, похожие на \"%s\" с расстоянием <= %d:\n", word, maxDist);
    if (similar) {
        for (int i = 0; i < count; i++) {
            int dist = levenshteinDistance(word, similar[i]);
            printf("  %s (расстояние %d)\n", similar[i], dist);
        }
        freeSimilarWords(similar, count);
    } else {
        printf("  нет\n");
    }
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        demo();
        return 0;
    }

    printf("=== Тестирование Distance Levenshtein ===\n");

    testDistance("kitten", "sitting", 3);
    testDistance("saturday", "sunday", 3);
    testDistance("", "abc", 3);
    testDistance("abc", "", 3);
    testDistance("abc", "abc", 0);
    testDistance("abcd", "abce", 1);

    testWithOperations("kitten", "sitting");
    testWithOperations("abc", "ac");
    testWithOperations("hello", "helo");

    testSimilar();

    printf("\nВсе тесты завершены.\n");
    return 0;
}
