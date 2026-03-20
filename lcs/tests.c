#include "lcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void testLCS(const char *s1, const char *s2) {
    printf("=== LCS: \"%s\" vs \"%s\" ===\n", s1, s2);

    int len = lcsLength(s1, s2);
    printf("Длина LCS (экономичный метод): %d\n", len);

    LCSResult *res = longestCommonSubsequence(s1, s2);
    if (!res) {
        printf("Ошибка выделения памяти\n");
        return;
    }

    printf("Длина LCS (полная таблица): %d\n", res->length);
    printf("LCS: \"%s\"\n", res->subsequence);

    printf("Таблица DP:\n");
    printLCSTable(res, s1, s2);

    printf("Визуализация:\n");
    highlightLCS(s1, s2, res->subsequence);

    freeLCSResult(res);
    printf("\n");
}
void testDiff() {
    char *lines1[] = {
        "#include <stdio.h>",
        "",
        "int main() {",
        "    printf(\"Hello\");",
        "    return 0;",
        "}"
    };
    int count1 = sizeof(lines1) / sizeof(lines1[0]);

    char *lines2[] = {
        "#include <stdio.h>",
        "#include <stdlib.h>",
        "",
        "int main() {",
        "    printf(\"Hello, World!\");",
        "    return 0;",
        "}"
    };
    int count2 = sizeof(lines2) / sizeof(lines2[0]);

    DiffResult *diff = diffLines(lines1, count1, lines2, count2);
    if (!diff) {
        printf("Ошибка diffLines\n");
        return;
    }

    printf("=== Diff двух фрагментов кода ===\n");
    printDiff(diff);
    freeDiffResult(diff);
}

void testEmpty() {
    printf("=== Пустые строки ===\n");
    testLCS("", "");
    testLCS("abc", "");
    testLCS("", "xyz");
}

void testSame() {
    printf("=== Одинаковые строки ===\n");
    testLCS("abcdef", "abcdef");
}

void demo() {
    printf("=== Демонстрация работы LCS и Diff ===\n\n");
    testLCS("AGGTAB", "GXTXAYB");
    testDiff();
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        demo();
        return 0;
    }

    // Запуск тестов
    printf("Запуск тестов LCS...\n");

    testEmpty();
    testSame();
    testLCS("ABCDGH", "AEDFHR");  // LCS = "ADH" длины 3
    testLCS("AGGTAB", "GXTXAYB"); // LCS = "GTAB" длины 4

    testDiff();

    printf("Все тесты выполнены.\n");
    return 0;
}
