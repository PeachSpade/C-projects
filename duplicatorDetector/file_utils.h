#ifndef FILE_UTILS_H
#define FILE_UTILS_H


typedef struct
{
    char path[512];
    char fileName[256];
    long fileSize;

} FileInfo;


// shared file counter
extern int totalFiles;


int scanDirectory(const char *folderPath,
                  FileInfo files[]);

int compareFiles(const char *file1,
                 const char *file2);

#endif