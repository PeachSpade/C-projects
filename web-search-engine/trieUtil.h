#ifndef TRIEUTIL_H
#define TRIEUTIL_H

struct trieStruct {
    int wordCount;
    struct trieStruct *childNodes[26];
};

int addWordOccurrence(const char *word, const int wordLength, struct trieStruct *root);
int freeTrieMemory(struct trieStruct *root);
int wordCount(struct trieStruct *root, const char *word);

#endif
