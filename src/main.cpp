#include <stdio.h>
#include "readCode.h"
#include "cpu.h"
#include "asm.h"
#include "MY_ASSERT.h"

const int NUM_TAGS = 20;

int main (int argc, char * argv[]) //secons argument is code file
{
    MY_ASSERT (argc != 3, "Too few command line arguments!");
    code_t codeInfo = {};

    codeInfo = readCode (argv[1]);

    MY_ASSERT (codeInfo.nStrs == 0, "There are no lines of code in your program");

    createBinFile (codeInfo.arrStrs, &codeInfo, argv[2], NUM_TAGS);

    getCode (argv[2], codeInfo, NUM_TAGS);
}

