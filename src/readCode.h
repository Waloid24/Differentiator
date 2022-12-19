#ifndef READ_CODE_H
#define READ_CODE_H

struct codeInfo
{
    size_t sizeFile;
    char * buf;
    int nStrs;
    char ** arrStrs;
};
typedef struct codeInfo code_t;

code_t readCode (const char * nameFile);

#endif