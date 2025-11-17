#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_WORD 20000
#define MAX_LEN 2000
#define HASH_SIZE 30007

typedef struct {
    char word[100];
    int count;
    int lines[500];
    int lineCount;
} IndexItem;

IndexItem table[MAX_WORD];
int tableSize = 0;
char stopwords[500][100];
int stopCount = 0;
int hashIndex[HASH_SIZE];

int isSentenceEnd(char c) {
    return (c == '.' || c == '!' || c == '?');
}

unsigned long hashFunc(const char *str) {
    unsigned long h = 5381;
    char c;
    while ((c = *str++))
        h = ((h << 5) + h) + c;
    return h % HASH_SIZE;
}

int findWord(char *w) {
    unsigned long h = hashFunc(w);
    int idx = hashIndex[h];
    if (idx == -1) return -1;
    if (strcmp(table[idx].word, w) == 0) return idx;
    return -1;
}

void addWordToHash(int index, char *w) {
    hashIndex[hashFunc(w)] = index;
}

int isStopWord(char *w) {
    for (int i = 0; i < stopCount; i++)
        if (strcmp(stopwords[i], w) == 0)
            return 1;
    return 0;
}

void addIndex(char *w, int line) {
    int idx = findWord(w);
    if (idx == -1) {
        strcpy(table[tableSize].word, w);
        table[tableSize].count = 1;
        table[tableSize].lines[0] = line;
        table[tableSize].lineCount = 1;
        addWordToHash(tableSize, w);
        tableSize++;
    } else {
        table[idx].count++;
        if (table[idx].lines[table[idx].lineCount - 1] != line)
            table[idx].lines[table[idx].lineCount++] = line;
    }
}

int cmp(const void *a, const void *b) {
    return strcmp(((IndexItem*)a)->word, ((IndexItem*)b)->word);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: ./index stop_word.txt van_ban.txt\n");
        return 1;
    }

    char *stopfile = argv[1];
    char *textfile = argv[2];

    for (int i = 0; i < HASH_SIZE; i++)
        hashIndex[i] = -1;

    FILE *fs = fopen(stopfile, "r");
    if (!fs) {
        printf("Khong mo duoc stopw.txt\n");
        return 1;
    }

    while (fscanf(fs, "%s", stopwords[stopCount]) == 1) {
        for (int i = 0; stopwords[stopCount][i]; i++)
            stopwords[stopCount][i] = tolower(stopwords[stopCount][i]);
        stopCount++;
    }
    fclose(fs);

    FILE *fv = fopen(textfile, "r");
    if (!fv) {
        printf("Khong mo duoc vanban.txt\n");
        return 1;
    }

    char lineStr[MAX_LEN];
    int lineNumber = 0;

    while (fgets(lineStr, sizeof(lineStr), fv)) {
        lineNumber++;
        int i = 0, pos = 0;
        char word[100];

        while (1) {
            char c = lineStr[i];
            if (isalpha(c)) {
                word[pos++] = c;
            } else {
                if (pos > 0) {
                    word[pos] = '\0';
                    int isFirst = 1;
                    for (int k = i - pos - 1; k >= 0; k--) {
                        if (isspace(lineStr[k])) continue;
                        if (isSentenceEnd(lineStr[k]))
                            isFirst = 1;
                        else
                            isFirst = 0;
                        break;
                    }
                    if (!isFirst && isupper(word[0])) {
                        pos = 0;
                        i++;
                        continue;
                    }
                    for (int t = 0; word[t]; t++)
                        word[t] = tolower(word[t]);
                    if (!isStopWord(word))
                        addIndex(word, lineNumber);
                }
                pos = 0;
            }
            if (c == '\0') break;
            i++;
        }
    }

    fclose(fv);
    qsort(table, tableSize, sizeof(IndexItem), cmp);

    for (int i = 0; i < tableSize; i++) {
        printf("%-12s %d  ", table[i].word, table[i].count);
        for (int j = 0; j < table[i].lineCount; j++) {
            printf("%d", table[i].lines[j]);
            if (j < table[i].lineCount - 1) printf(", ");
        }
        printf("\n");
    }

    return 0;
}
