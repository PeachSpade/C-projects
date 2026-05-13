#include "trieUtil.h"
#include <stdlib.h>
#include <stdio.h>

int addWordOccurrence(const char *word, const int wordLength, struct trieStruct *root) {
    struct trieStruct *currentNode = root;

    for (int i = 0; i < wordLength; i++) {
        char letter = word[i];
        int indexPos = letter - 'a';

        if (indexPos < 0 || indexPos >= 26) {
            continue;
        }

        if (currentNode->childNodes[indexPos] == NULL) {
            currentNode->childNodes[indexPos] = malloc(sizeof(struct trieStruct));

            if (currentNode->childNodes[indexPos] == NULL) {
                fprintf(stderr, "Error: Could not allocate memory!\n");
                return 55;
            }

            currentNode->childNodes[indexPos]->wordCount = 0;

            for (int j = 0; j < 26; j++) {
                currentNode->childNodes[indexPos]->childNodes[j] = NULL;
            }
        }

        currentNode = currentNode->childNodes[indexPos];
    }

    currentNode->wordCount += 1;
    return 0;
}

int freeTrieMemory(struct trieStruct *node) {
    if (node == NULL) {
        return 0;
    }

    for (int i = 0; i < 26; i++) {
        if (node->childNodes[i] != NULL) {
            freeTrieMemory(node->childNodes[i]);
        }
    }

    free(node);
    return 0;
}


int wordCount(struct trieStruct *root, const char *word) {
    struct trieStruct *currentNode = root;

    for (int i = 0; word[i]; i++) {
        char letter = word[i];
        int indexPos = letter - 'a';

        if (indexPos < 0 || indexPos > 25) {
            return 0;
        }

        if (!currentNode->childNodes[indexPos]) {
            return 0;
        }

        currentNode = currentNode->childNodes[indexPos];
    }

    return currentNode->wordCount;
}