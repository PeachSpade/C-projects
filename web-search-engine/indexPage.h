#ifndef INDEXPAGE_H
#define INDEXPAGE_H

#include "trieUtil.h"

#define MAX_ADDR_LENGTH 1000
#define MAX_QUERY_TERMS 50


struct listNode {
    char addr[MAX_ADDR_LENGTH];
    struct listNode *next;
};

struct trieStruct *indexPage(const char *url, int *pTotalNumTerms);
int contains(const struct listNode *pNode, const char *addr);
void insertBack(struct listNode *pNode, const char *addr);
void destroyList(struct listNode *pNode);
int validQuery(const char *query);

#endif
