#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include "file_utils.h"

#define MAX_FILES 10000


// shared file count across recursion
int totalFiles = 0;


// recursively scans folders
int scanDirectory(const char *folderPath,
                  FileInfo files[])
{
    WIN32_FIND_DATA fileData;

    char searchPath[512];

    snprintf(searchPath,
             sizeof(searchPath),
             "%s\\*",
             folderPath);

    HANDLE hFind =
        FindFirstFile(searchPath,
                      &fileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        return totalFiles;
    }

    do
    {
        // skip current and parent folders
        if (strcmp(fileData.cFileName, ".") == 0 ||
            strcmp(fileData.cFileName, "..") == 0)
        {
            continue;
        }

        char fullPath[512];

        snprintf(fullPath,
                 sizeof(fullPath),
                 "%s\\%s",
                 folderPath,
                 fileData.cFileName);

        // recursively scan folders
        if (fileData.dwFileAttributes &
            FILE_ATTRIBUTE_DIRECTORY)
        {
            scanDirectory(fullPath,
                          files);
        }

        else
        {
            // avoid overflowing array
            if (totalFiles >= MAX_FILES)
            {
                continue;
            }

            strcpy(files[totalFiles].path,
                   fullPath);

            strcpy(files[totalFiles].fileName,
                   fileData.cFileName);

            LARGE_INTEGER size;

            size.HighPart =
                fileData.nFileSizeHigh;

            size.LowPart =
                fileData.nFileSizeLow;

            files[totalFiles].fileSize =
                size.QuadPart;

            totalFiles++;
        }

    } while (FindNextFile(hFind,
                          &fileData));

    FindClose(hFind);

    return totalFiles;
}


// compares file contents byte-by-byte
int compareFiles(const char *file1,
                 const char *file2)
{
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");

    if (fp1 == NULL || fp2 == NULL)
    {
        return 0;
    }

    int byte1;
    int byte2;

    do
    {
        byte1 = fgetc(fp1);
        byte2 = fgetc(fp2);

        if (byte1 != byte2)
        {
            fclose(fp1);
            fclose(fp2);

            return 0;
        }

    } while (byte1 != EOF &&
             byte2 != EOF);

    fclose(fp1);
    fclose(fp2);

    return 1;
}