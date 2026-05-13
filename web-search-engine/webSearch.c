#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "webUtil.h"
#include "indexPage.h"
#include "trieUtil.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s urlFile maxPages seed\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int maxPages = atoi(argv[2]);
    long seed = atol(argv[3]);

    srand(seed);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open %s\n", filename);
        return 1;
    }

    printf("Indexing...\n");

    struct trieStruct *tries[maxPages];
    int totalWords[maxPages];
    char *urls[maxPages];
    int pageCount = 0;

    struct listNode *visited = NULL;

    char url[1024];
    char newURL[1024];
    int hopsLimit;

    
    while (pageCount < maxPages && fscanf(fp, "%1023s %d", url, &hopsLimit) == 2) {
        int hops = 0;
        
        while (hops <= hopsLimit && pageCount < maxPages) {

            if (!contains(visited, url)) {
                // add the url to visited list
                if (visited == NULL) {
                    visited = malloc(sizeof(struct listNode));

                    if (visited == NULL) {
                        fprintf(stderr, "Could not allocate memory for visited list.\n");
                        fclose(fp);
                        return 1;
                    }
                    strncpy(visited->addr, url, MAX_ADDR_LENGTH);
                    visited->addr[MAX_ADDR_LENGTH - 1] = '\0';
                    visited->next = NULL;
                } else {
                    insertBack(visited, url);
                }
                
                int totalNumTerms = 0;
                struct trieStruct *root = indexPage(url, &totalNumTerms);
                
                if (root != NULL) {
                    tries[pageCount] = root;
                    totalWords[pageCount] = totalNumTerms;
                    urls[pageCount] = strdup(url);
                    pageCount++;
                }
            }

            // goto the next hop
            hops++;

            if (hops > hopsLimit || pageCount >= maxPages) {
                break;
            }

            // dead end if no link found
            if (!getLink(url, newURL, sizeof(newURL))) {
                break;
            }

            strncpy(url, newURL, sizeof(url));
            url[sizeof(url) - 1] = '\0';
        }
    }

    fclose(fp);

    // Free memory for visited url list
    destroyList(visited);

    // PART 2
    fflush(stdout);
    char query[128];

    printf("\nEnter a web query: ");

    while (1) {
        fflush(stdout);

        if (!fgets(query, sizeof(query), stdin)) {
            break;
        }

        query[strcspn(query, "\n")] = '\0';

        if (strlen(query) == 0) {
            break;
        }

        if (!validQuery(query)) {
            fprintf(stderr, "Please enter a query containing only lower-case letters.\n");
            printf("\nEnter a web query: ");
            continue;
        }

        printf("Query is \"%s\".\n", query);

        char *terms[MAX_QUERY_TERMS];
        int termCount = 0;

        char *token = strtok(query, " ");
        while (token && termCount < MAX_QUERY_TERMS) {
            if (strlen(token) > 0) {
                terms[termCount++] = token;
            }
            token = strtok(NULL, " ");
        }

        printf("IDF scores are\n");

        double idfScores[MAX_QUERY_TERMS];

        for (int i = 0; i < termCount; i++) {
            int docsFrequency = 0;
            
            // count the number of documents containing the term
            for (int j = 0; j < pageCount; j++) {
                if (wordCount(tries[j], terms[i]) > 0) {
                    docsFrequency++;
                }
            }

            idfScores[i] = log((1.0 + pageCount) / (1.0 + docsFrequency));
            printf("IDF(%s): %.7f\n", terms[i], idfScores[i]);
        }

        double pageScores[pageCount];

        for (int j = 0; j < pageCount; j++) {
            double score = 0.0;

            for (int i = 0; i < termCount; i++) {
                int frequency = wordCount(tries[j], terms[i]);
                if (frequency > 0) {
                    double tf = (double)frequency / totalWords[j];
                    score += tf * idfScores[i];
                }
            }

            pageScores[j] = score;
        }

        int order[pageCount];
        for (int i = 0; i < pageCount; i++) {
            order[i] = i;
        }

        for (int i = 0; i < pageCount; i++) {
            for (int j = i + 1; j < pageCount; j++){
                if (pageScores[order[j]] > pageScores[order[i]]) {
                    int temp = order[i];
                    order[i] = order[j];
                    order[j] = temp;
                }
            }
        }

        printf("Web pages:\n");

         
        int pageID = 0;
        for (int i = 0; i < pageCount && pageID < 3; i++) {
            int indexPos = order[i];
            if (pageScores[indexPos] > 0) {
                printf("%d. %s (score: %.4f)\n", pageID + 1, urls[indexPos], pageScores[indexPos]);
                pageID++;
            }
        }
        printf("\nEnter a web query: ");
    }
    printf("Exiting the program\n");

    for (int i = 0; i < pageCount; i++) {
        freeTrieMemory(tries[i]);
        free(urls[i]);
    }

    return 0;
}
