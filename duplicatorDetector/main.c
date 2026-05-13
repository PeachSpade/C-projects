#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_utils.h"

#define MAX_FILES 10000
#define MAX_GROUPS 10


// stores scanned file information
FileInfo files[MAX_FILES];


// filters out unnecessary/system files
int shouldIgnoreFile(char *fileName)
{
    char *extension = strrchr(fileName, '.');

    // ignore files with no extension
    if (extension == NULL)
    {
        return 1;
    }

    // allow only useful/common user file types
    if (strcmp(extension, ".pdf") == 0 ||
        strcmp(extension, ".docx") == 0 ||
        strcmp(extension, ".txt") == 0 ||
        strcmp(extension, ".png") == 0 ||
        strcmp(extension, ".jpg") == 0 ||
        strcmp(extension, ".jpeg") == 0 ||
        strcmp(extension, ".zip") == 0 ||
        strcmp(extension, ".mp4") == 0 ||
        strcmp(extension, ".mp3") == 0 ||
        strcmp(extension, ".exe") == 0)
    {
        return 0;
    }

    // ignore everything else
    return 1;
}


// removes patterns like (1), (2), etc.
void normalizeFileName(char *original,
                       char *cleaned)
{
    strcpy(cleaned, original);

    char *openBracket = strrchr(cleaned, '(');
    char *closeBracket = strrchr(cleaned, ')');

    // detect copied file names
    if (openBracket != NULL &&
        closeBracket != NULL &&
        closeBracket > openBracket)
    {
        int validNumber = 1;

        for (char *p = openBracket + 1;
             p < closeBracket;
             p++)
        {
            if (*p < '0' || *p > '9')
            {
                validNumber = 0;
                break;
            }
        }

        if (validNumber && openBracket > cleaned)
        {
            *(openBracket - 1) = '\0';
        }
    }
}


// cleaner grouped filename detection
void findCopiedNames()
{
    int groupNumber = 1;
    int found = 0;

    int alreadyPrinted[MAX_FILES] = {0};

    printf("\n=================================================================\n");
    printf("POSSIBLE COPIED FILENAMES\n");
    printf("=================================================================\n");

    for (int i = 0; i < totalFiles; i++)
    {
        // skip tiny files
        if (files[i].fileSize < 50000)
        {
            continue;
        }

        if (alreadyPrinted[i])
        {
            continue;
        }

        char cleaned1[256];

        normalizeFileName(files[i].fileName,
                          cleaned1);

        int groupFound = 0;

        for (int j = i + 1; j < totalFiles; j++)
        {
            // skip tiny files
            if (files[j].fileSize < 50000)
            {
                continue;
            }

            char cleaned2[256];

            normalizeFileName(files[j].fileName,
                              cleaned2);

            // filenames must match AND sizes must be close
            if (strcmp(cleaned1, cleaned2) == 0 &&
                abs(files[i].fileSize -
                    files[j].fileSize) < 10000)
            {
                if (!groupFound)
                {
                    found = 1;
                    groupFound = 1;

                    if (groupNumber > MAX_GROUPS)
                    {
                        printf("\nToo many copied filename groups found.\n");
                        return;
                    }

                    printf("\nGroup %d:\n",
                           groupNumber++);

                    printf("- %s\n",
                           files[i].path);

                    alreadyPrinted[i] = 1;
                }

                printf("- %s\n",
                       files[j].path);

                alreadyPrinted[j] = 1;
            }
        }

        if (groupFound)
        {
            printf("-----------------------------------------------------------------\n");
        }
    }

    if (!found)
    {
        printf("No copied filename patterns found.\n");
    }
}


// grouped duplicate content detection
void findDuplicateContents()
{
    int groupNumber = 1;
    int found = 0;

    int alreadyPrinted[MAX_FILES] = {0};

    printf("\n=================================================================\n");
    printf("DUPLICATE FILE CONTENTS\n");
    printf("=================================================================\n");

    for (int i = 0; i < totalFiles; i++)
    {
        // skip tiny files
        if (files[i].fileSize < 50000)
        {
            continue;
        }

        if (alreadyPrinted[i])
        {
            continue;
        }

        int groupFound = 0;

        for (int j = i + 1; j < totalFiles; j++)
        {
            // skip tiny files
            if (files[j].fileSize < 50000)
            {
                continue;
            }

            // allow small size differences
            if (abs(files[i].fileSize -
                    files[j].fileSize) < 20)
            {
                // compare actual file contents
                if (compareFiles(files[i].path,
                                 files[j].path))
                {
                    if (!groupFound)
                    {
                        found = 1;
                        groupFound = 1;

                        if (groupNumber > MAX_GROUPS)
                        {
                            printf("\nToo many duplicate groups found.\n");
                            return;
                        }

                        printf("\nGroup %d:\n",
                               groupNumber++);

                        printf("- %s\n",
                               files[i].path);

                        alreadyPrinted[i] = 1;
                    }

                    printf("- %s\n",
                           files[j].path);

                    alreadyPrinted[j] = 1;
                }
            }
        }

        if (groupFound)
        {
            printf("-----------------------------------------------------------------\n");
        }
    }

    if (!found)
    {
        printf("No duplicate file contents found.\n");
    }

    printf("=================================================================\n");
}


int main()
{
    char folderPath[512];

    printf("Enter folder path to scan: ");
    fflush(stdout);

    fgets(folderPath,
          sizeof(folderPath),
          stdin);

    // remove newline character
    folderPath[strcspn(folderPath, "\n")] = 0;

    // reset file count before scanning
    totalFiles = 0;

    scanDirectory(folderPath,
                  files);

    // dynamically allocate filtered array
    FileInfo *filteredFiles =
        malloc(sizeof(FileInfo) * MAX_FILES);

    if (filteredFiles == NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }

    int filteredCount = 0;

    // remove ignored files
    for (int i = 0; i < totalFiles; i++)
    {
        if (!shouldIgnoreFile(files[i].fileName))
        {
            filteredFiles[filteredCount++] =
                files[i];
        }
    }

    // copy filtered files back
    for (int i = 0; i < filteredCount; i++)
    {
        files[i] = filteredFiles[i];
    }

    totalFiles = filteredCount;

    printf("\nScanned %d files.\n",
           totalFiles);

    findCopiedNames();

    findDuplicateContents();

    free(filteredFiles);

    return 0;
}