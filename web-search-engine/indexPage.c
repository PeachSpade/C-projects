#include "indexPage.h"
#include "webUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct trieStruct *indexPage(const char *url, int *pTotalNumTerms) {
    int bufferSize = 300000;
    char *buffer = malloc(bufferSize);
    char currentWord[128];
    int indexPos = 0;
    int totalTerms = 0;

    if (buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate memory.\n");
        return NULL;
    }

    struct trieStruct *rootNode = malloc(sizeof(struct trieStruct));
    if (rootNode == NULL) {
        fprintf(stderr, "Error: Could not allocate memory.\n");
        free(buffer);
        return NULL;
    }

    rootNode->wordCount = 0;
    for (int i = 0; i < 26; i++) {
        rootNode->childNodes[i] = NULL;
    }

    int bytesRead = getText(url, buffer, bufferSize);
    if (bytesRead < 1) {
        fprintf(stderr, "Error: cannot get text from url!\n");
        free(buffer);
        free(rootNode);
        return NULL;
    }

    
    printf("%s\n", url);


    for (int i = 0; i < bytesRead; i++) {
        if (isalpha((unsigned char)buffer[i])) {
            if (indexPos < 127) {
                currentWord[indexPos] = (char)tolower((unsigned char)buffer[i]);
                indexPos++;
            }
        } else if (indexPos > 0) {
            currentWord[indexPos] = '\0';
            printf("\t%s\n", currentWord);
            addWordOccurrence(currentWord, indexPos, rootNode);
            totalTerms++;
            indexPos = 0;
        }
    }

    // Last word if it ended at the end of the buffer 
    if (indexPos > 0) {
        currentWord[indexPos] = '\0';
        printf("\t%s\n", currentWord);
        addWordOccurrence(currentWord, indexPos, rootNode);
        totalTerms++;
    }

    free(buffer);

    if (pTotalNumTerms != NULL) {
        *pTotalNumTerms = totalTerms;
    }

    return rootNode;
}



int contains(const struct listNode *pNode, const char *addr) {
    if (pNode != NULL) {
        if (strcmp(addr, pNode->addr) == 0) {
            return 1;
        } else {
            return contains(pNode->next, addr);
        }
    } else {
        return 0;
    }
}

void insertBack(struct listNode *pNode, const char *addr) {
    if (pNode->next == NULL) {
        struct listNode *newNode = malloc(sizeof(struct listNode));

        if (newNode == NULL) {
            fprintf(stderr, "Could not allocate memory!\n");
            return;
        }

        strncpy(newNode->addr, addr, MAX_ADDR_LENGTH);
        newNode->addr[MAX_ADDR_LENGTH - 1] = '\0';
        newNode->next = NULL;
        pNode->next = newNode;
    } else {
        insertBack(pNode->next, addr);
    }
}

void destroyList(struct listNode *pNode) {
    if (pNode == NULL) {
        return;
    }

    destroyList(pNode->next);
    free(pNode);
}


int validQuery(const char *query) {
    for (int i = 0; query[i]; i++) {
        if (!(query[i] == ' ' || (query[i] >= 'a' && query[i] <= 'z'))) {
            return 0;
        }
    }
    return 1;
}
