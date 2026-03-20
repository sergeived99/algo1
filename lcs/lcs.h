#ifndef LCS_H
#define LCS_H

#include <stddef.h>

typedef struct {
    int length;
    char *subsequence;
    int **dpTable;
    int rows;
    int cols;
} LCSResult;

typedef struct {
    char type;
    char *line;
} DiffLine;

typedef struct {
    DiffLine *lines;
    int count;
} DiffResult;

int lcsLength(const char *s1, const char *s2);

LCSResult *longestCommonSubsequence(const char *s1, const char *s2);

char *reconstructLCS(int **dp, const char *s1, const char *s2, int i, int j);

void printLCSTable(LCSResult *result, const char *s1, const char *s2);

void highlightLCS(const char *s1, const char *s2, const char *lcs);

DiffResult *diffLines(char **lines1, int count1, char **lines2, int count2);

DiffResult *diffFiles(const char *file1, const char *file2);

void printDiff(DiffResult *result);

void freeLCSResult(LCSResult *result);
void freeDiffResult(DiffResult *result);

#endif // LCS_H
