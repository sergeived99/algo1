#include "lcs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int maxInt(int a, int b) {
    return (a > b) ? a : b;
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

int lcsLength(const char *s1, const char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);

    int *prev = (int *)calloc(n + 1, sizeof(int));
    int *curr = (int *)calloc(n + 1, sizeof(int));
    if (!prev || !curr) {
        free(prev);
        free(curr);
        return 0;
    }

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = maxInt(prev[j], curr[j - 1]);
            }
        }
        int *tmp = prev;
        prev = curr;
        curr = tmp;
    }

    int result = prev[n];
    free(prev);
    free(curr);
    return result;
}

LCSResult *longestCommonSubsequence(const char *s1, const char *s2) {
    int m = strlen(s1);
    int n = strlen(s2);

    LCSResult *res = (LCSResult *)malloc(sizeof(LCSResult));
    if (!res) return NULL;

    res->rows = m + 1;
    res->cols = n + 1;
    res->dpTable = allocateTable(res->rows, res->cols);
    if (!res->dpTable) {
        free(res);
        return NULL;
    }
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0) {
                res->dpTable[i][j] = 0;
            } else if (s1[i - 1] == s2[j - 1]) {
                res->dpTable[i][j] = res->dpTable[i - 1][j - 1] + 1;
            } else {
                res->dpTable[i][j] = maxInt(res->dpTable[i - 1][j], res->dpTable[i][j - 1]);
            }
        }
    }

    res->length = res->dpTable[m][n];
    res->subsequence = reconstructLCS(res->dpTable, s1, s2, m, n);
    if (!res->subsequence) {
        freeTable(res->dpTable, res->rows);
        free(res);
        return NULL;
    }

    return res;
}

char *reconstructLCS(int **dp, const char *s1, const char *s2, int i, int j) {
    int len = dp[i][j];
    char *lcs = (char *)malloc((len + 1) * sizeof(char));
    if (!lcs) return NULL;

    int idx = len;
    lcs[idx] = '\0';

    while (i > 0 && j > 0) {
        if (s1[i - 1] == s2[j - 1]) {
            lcs[--idx] = s1[i - 1];
            i--;
            j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }
    return lcs;
}
void printLCSTable(LCSResult *result, const char *s1, const char *s2) {
    int m = result->rows - 1;
    int n = result->cols - 1;

    printf("    ");
    for (int j = 0; j < n; j++) {
        printf(" %c  ", s2[j]);
    }
    printf("\n");

    for (int i = 0; i <= m; i++) {
        if (i == 0) {
            printf("  ");
        } else {
            printf("%c ", s1[i - 1]);
        }

        for (int j = 0; j <= n; j++) {
            printf("%2d ", result->dpTable[i][j]);
        }
        printf("\n");
    }
}
void highlightLCS(const char *s1, const char *s2, const char *lcs) {
    int i = 0, j = 0, k = 0;
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int lcsLen = strlen(lcs);

    printf("s1: ");
    while (i < len1 && k < lcsLen) {
        if (s1[i] == lcs[k]) {
            printf("[%c]", s1[i]);
            k++;
        } else {
            printf("%c", s1[i]);
        }
        i++;
    }
    while (i < len1) {
        printf("%c", s1[i++]);
    }
    printf("\n");

    j = 0;
    k = 0;
    printf("s2: ");
    while (j < len2 && k < lcsLen) {
        if (s2[j] == lcs[k]) {
            printf("[%c]", s2[j]);
            k++;
        } else {
            printf("%c", s2[j]);
        }
        j++;
    }
    while (j < len2) {
        printf("%c", s2[j++]);
    }
    printf("\n");
}
static int **computeLCSTableForLines(char **lines1, int count1, char **lines2, int count2) {
    int **dp = allocateTable(count1 + 1, count2 + 1);
    if (!dp) return NULL;

    for (int i = 0; i <= count1; i++) {
        for (int j = 0; j <= count2; j++) {
            if (i == 0 || j == 0) {
                dp[i][j] = 0;
            } else if (strcmp(lines1[i - 1], lines2[j - 1]) == 0) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = maxInt(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp;
}
DiffResult *diffLines(char **lines1, int count1, char **lines2, int count2) {
    int **dp = computeLCSTableForLines(lines1, count1, lines2, count2);
    if (!dp) return NULL;

    DiffResult *result = (DiffResult *)malloc(sizeof(DiffResult));
    if (!result) {
        freeTable(dp, count1 + 1);
        return NULL;
    }
    int maxSize = count1 + count2;
    DiffLine *diffLines = (DiffLine *)malloc(maxSize * sizeof(DiffLine));
    if (!diffLines) {
        freeTable(dp, count1 + 1);
        free(result);
        return NULL;
    }

    int i = count1, j = count2;
    int pos = maxSize - 1;

    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && strcmp(lines1[i - 1], lines2[j - 1]) == 0) {
            diffLines[pos].type = ' ';
            diffLines[pos].line = strdup(lines1[i - 1]);
            i--;
            j--;
            pos--;
        } else if (j > 0 && (i == 0 || dp[i][j - 1] >= dp[i - 1][j])) {
            diffLines[pos].type = '+';
            diffLines[pos].line = strdup(lines2[j - 1]);
            j--;
            pos--;
        } else {
            diffLines[pos].type = '-';
            diffLines[pos].line = strdup(lines1[i - 1]);
            i--;
            pos--;
        }
    }

    int resultCount = maxSize - 1 - pos;
    result->lines = (DiffLine *)malloc(resultCount * sizeof(DiffLine));
    if (!result->lines) {
        for (int k = pos + 1; k < maxSize; k++) free(diffLines[k].line);
        free(diffLines);
        freeTable(dp, count1 + 1);
        free(result);
        return NULL;
    }

    memcpy(result->lines, diffLines + pos + 1, resultCount * sizeof(DiffLine));
    result->count = resultCount;

    free(diffLines);
    freeTable(dp, count1 + 1);
    return result;
}
static char **readFileLines(const char *filename, int *count) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    char **lines = NULL;
    int capacity = 10;
    int size = 0;
    lines = (char **)malloc(capacity * sizeof(char *));
    if (!lines) {
        fclose(f);
        return NULL;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), f)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        if (size >= capacity) {
            capacity *= 2;
            char **newLines = (char **)realloc(lines, capacity * sizeof(char *));
            if (!newLines) {
                for (int i = 0; i < size; i++) free(lines[i]);
                free(lines);
                fclose(f);
                return NULL;
            }
            lines = newLines;
        }
        lines[size] = strdup(buffer);
        if (!lines[size]) {
            for (int i = 0; i < size; i++) free(lines[i]);
            free(lines);
            fclose(f);
            return NULL;
        }
        size++;
    }

    fclose(f);
    *count = size;
    return lines;
}

static void freeFileLines(char **lines, int count) {
    if (!lines) return;
    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}

DiffResult *diffFiles(const char *file1, const char *file2) {
    int count1, count2;
    char **lines1 = readFileLines(file1, &count1);
    char **lines2 = readFileLines(file2, &count2);

    if (!lines1 || !lines2) {
        freeFileLines(lines1, count1);
        freeFileLines(lines2, count2);
        return NULL;
    }

    DiffResult *result = diffLines(lines1, count1, lines2, count2);

    freeFileLines(lines1, count1);
    freeFileLines(lines2, count2);
    return result;
}

void printDiff(DiffResult *result) {
    if (!result) return;
    for (int i = 0; i < result->count; i++) {
        printf("%c %s\n", result->lines[i].type, result->lines[i].line);
    }
}

void freeLCSResult(LCSResult *result) {
    if (!result) return;
    freeTable(result->dpTable, result->rows);
    free(result->subsequence);
    free(result);
}

void freeDiffResult(DiffResult *result) {
    if (!result) return;
    for (int i = 0; i < result->count; i++) {
        free(result->lines[i].line);
    }
    free(result->lines);
    free(result);
}
