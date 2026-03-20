#include "levenshtein.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int minInt(int a, int b) {
    return (a < b) ? a : b;
}

static int minOfThree(int a, int b, int c) {
    return minInt(minInt(a, b), c);
}

static int **allocateTable(int rows, int cols) {
    int **table = (int **)malloc(rows * sizeof(int *));
    if (!table) return NULL;
    for (int i = 0; i < rows; i++) {
        table[i] = (int *)malloc(cols * sizeof(int));
        if (!table[i]) {
            for (int k = 0; k < i; k++) free(table[k]);
            free(table);
            return NULL;
        }
    }
    return table;
}

static void freeTable(int **table, int rows) {
    if (!table) return;
    for (int i = 0; i < rows; i++) {
        free(table[i]);
    }
    free(table);
}

const char *operationName(OperationType type) {
    switch (type) {
        case OP_NONE:    return "NONE";
        case OP_INSERT:  return "INSERT";
        case OP_DELETE:  return "DELETE";
        case OP_REPLACE: return "REPLACE";
        default:         return "UNKNOWN";
    }
}

int levenshteinDistance(const char *s1, const char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);

    int *prev = (int *)malloc((n + 1) * sizeof(int));
    int *curr = (int *)malloc((n + 1) * sizeof(int));
    if (!prev || !curr) {
        free(prev);
        free(curr);
        return -1;
    }

    for (int j = 0; j <= n; j++) prev[j] = j;

    for (int i = 1; i <= m; i++) {
        curr[0] = i;
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                curr[j] = prev[j - 1];
            } else {
                curr[j] = 1 + minOfThree(prev[j],
                                          curr[j - 1],
                                          prev[j - 1]);
            }
        }
        // swap
        int *tmp = prev;
        prev = curr;
        curr = tmp;
    }

    int result = prev[n];
    free(prev);
    free(curr);
    return result;
}

EditResult *levenshteinWithOperations(const char *s1, const char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);
    int **dp = allocateTable(m + 1, n + 1);
    if (!dp) return NULL;
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0) {
                dp[i][j] = j;
            } else if (j == 0) {
                dp[i][j] = i;
            } else if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + minOfThree(dp[i - 1][j],
                                           dp[i][j - 1],
                                           dp[i - 1][j - 1]);
            }
        }
    }

    int distance = dp[m][n];
    EditOperation *ops = NULL;
    int opCount = 0;
    int maxOps = m + n;
    ops = (EditOperation *)malloc(maxOps * sizeof(EditOperation));
    if (!ops) {
        freeTable(dp, m + 1);
        return NULL;
    }

    int i = m, j = n;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && s1[i - 1] == s2[j - 1]) {
            i--;
            j--;
        } else if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + 1) {
            ops[opCount].type = OP_REPLACE;
            ops[opCount].position = i - 1;
            ops[opCount].oldChar = s1[i - 1];
            ops[opCount].newChar = s2[j - 1];
            opCount++;
            i--;
            j--;
        } else if (i > 0 && dp[i][j] == dp[i - 1][j] + 1) {
            ops[opCount].type = OP_DELETE;
            ops[opCount].position = i - 1;
            ops[opCount].oldChar = s1[i - 1];
            ops[opCount].newChar = '\0';
            opCount++;
            i--;
        } else {
            ops[opCount].type = OP_INSERT;
            ops[opCount].position = i;
            ops[opCount].oldChar = '\0';
            ops[opCount].newChar = s2[j - 1];
            opCount++;
            j--;
        }
    }

    EditResult *result = (EditResult *)malloc(sizeof(EditResult));
    if (!result) {
        free(ops);
        freeTable(dp, m + 1);
        return NULL;
    }

    result->distance = distance;
    result->operationCount = opCount;
    result->operations = (EditOperation *)malloc(opCount * sizeof(EditOperation));
    if (!result->operations) {
        free(ops);
        free(result);
        freeTable(dp, m + 1);
        return NULL;
    }

    for (int k = 0; k < opCount; k++) {
        result->operations[k] = ops[opCount - 1 - k];
    }

    free(ops);
    freeTable(dp, m + 1);
    return result;
}

void printEditTable(int **dp, const char *s1, const char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);

    printf("    ");
    for (int j = 0; j < n; j++) {
        printf(" %c ", s2[j]);
    }
    printf("\n");

    for (int i = 0; i <= m; i++) {
        if (i == 0) {
            printf("  ");
        } else {
            printf("%c ", s1[i - 1]);
        }
        for (int j = 0; j <= n; j++) {
            printf("%2d ", dp[i][j]);
        }
        printf("\n");
    }
}
void printOperations(EditResult *result, const char *s1, const char *s2) {
    if (!result) return;
    printf("Операции (%d шт.):\n", result->operationCount);
    for (int k = 0; k < result->operationCount; k++) {
        EditOperation *op = &result->operations[k];
        printf("  %d. ", k + 1);
        switch (op->type) {
            case OP_DELETE:
                printf("DELETE '%c' из позиции %d\n", op->oldChar, op->position);
                break;
            case OP_INSERT:
                printf("INSERT '%c' в позицию %d\n", op->newChar, op->position);
                break;
            case OP_REPLACE:
                printf("REPLACE '%c' -> '%c' в позиции %d\n", op->oldChar, op->newChar, op->position);
                break;
            default:
                break;
        }
    }
}
static int findPosByOriginalIndex(char *str, int *origIndices, int len, int origIdx) {
    for (int p = 0; p < len; p++) {
        if (origIndices[p] == origIdx) return p;
    }
    return -1;
}

void printTransformation(EditResult *result, const char *s1, const char *s2) {
    if (!result) return;

    int maxLen = strlen(s1) + result->operationCount + 5;
    char *current = (char *)malloc(maxLen + 1);
    int *origIndices = (int *)malloc(maxLen * sizeof(int));
    if (!current || !origIndices) {
        free(current);
        free(origIndices);
        return;
    }

    int len = strlen(s1);
    strcpy(current, s1);
    for (int i = 0; i < len; i++) origIndices[i] = i;

    printf("\nПошаговое преобразование:\n");
    printf("  Начало:  \"%s\"\n", current);

    for (int k = 0; k < result->operationCount; k++) {
        EditOperation *op = &result->operations[k];

        switch (op->type) {
            case OP_DELETE: {
                int pos = findPosByOriginalIndex(current, origIndices, len, op->position);
                if (pos < 0) {
                    printf("Ошибка: символ с исходным индексом %d не найден\n", op->position);
                    goto cleanup;
                }
                for (int i = pos; i < len - 1; i++) {
                    current[i] = current[i + 1];
                    origIndices[i] = origIndices[i + 1];
                }
                len--;
                current[len] = '\0';
                break;
            }
            case OP_INSERT: {
                int insertPos;
                if (op->position >= strlen(s1)) {
                    insertPos = len;
                } else {
                    insertPos = findPosByOriginalIndex(current, origIndices, len, op->position);
                    if (insertPos < 0) {
                        printf("Ошибка: символ с исходным индексом %d не найден для вставки перед ним\n", op->position);
                        goto cleanup;
                    }
                }
                for (int i = len; i > insertPos; i--) {
                    current[i] = current[i - 1];
                    origIndices[i] = origIndices[i - 1];
                }
                current[insertPos] = op->newChar;
                origIndices[insertPos] = -1;
                len++;
                current[len] = '\0';
                break;
            }
            case OP_REPLACE: {
                int pos = findPosByOriginalIndex(current, origIndices, len, op->position);
                if (pos < 0) {
                    printf("Ошибка: символ с исходным индексом %d не найден для замены\n", op->position);
                    goto cleanup;
                }
                current[pos] = op->newChar;
                origIndices[pos] = -1;
                break;
            }
            default:
                break;
        }
        printf("  Шаг %d:   \"%s\"  (%s", k + 1, current, operationName(op->type));
        if (op->type == OP_DELETE) {
            printf(" '%c' из %d)", op->oldChar, op->position);
        } else if (op->type == OP_INSERT) {
            printf(" '%c' в %d)", op->newChar, op->position);
        } else if (op->type == OP_REPLACE) {
            printf(" '%c'->'%c' в %d)", op->oldChar, op->newChar, op->position);
        }
        printf("\n");
    }

    printf("  Итог:    \"%s\"\n", current);

cleanup:
    free(current);
    free(origIndices);
}

char **findSimilarWords(const char *word, char **dictionary, int dictSize,
                        int maxDistance, int *resultCount) {
    if (!word || !dictionary || dictSize <= 0 || maxDistance < 0 || !resultCount) {
        return NULL;
    }

    int count = 0;
    for (int i = 0; i < dictSize; i++) {
        int dist = levenshteinDistance(word, dictionary[i]);
        if (dist >= 0 && dist <= maxDistance) {
            count++;
        }
    }

    if (count == 0) {
        *resultCount = 0;
        return NULL;
    }

    char **result = (char **)malloc(count * sizeof(char *));
    if (!result) {
        *resultCount = 0;
        return NULL;
    }

    int idx = 0;
    for (int i = 0; i < dictSize; i++) {
        int dist = levenshteinDistance(word, dictionary[i]);
        if (dist >= 0 && dist <= maxDistance) {
            result[idx] = strdup(dictionary[i]);
            if (!result[idx]) {
                for (int j = 0; j < idx; j++) free(result[j]);
                free(result);
                *resultCount = 0;
                return NULL;
            }
            idx++;
        }
    }

    *resultCount = count;
    return result;
}
void freeSimilarWords(char **words, int count) {
    if (!words) return;
    for (int i = 0; i < count; i++) {
        free(words[i]);
    }
    free(words);
}
void freeEditResult(EditResult *result) {
    if (!result) return;
    free(result->operations);
    free(result);
}
