#include "syscall.h"

#define stdin 0
#define stdout 1
#define ENDL PrintChar('\n')
int main() {
    char sourceFile[9], desFile[9], bufferSource[100];
    int len = 0;
    int i;
    int sourceFileId, desFileId;

    PrintString("Type source file (use Enter to end typing):\n");
    ReadString(sourceFile, 2);
    

    PrintString("Type destination file (use Enter to end typing):\n");
    ReadString(desFile, 5);
    

    sourceFileId = Open(sourceFile);
    Read(bufferSource, 50, sourceFileId);
    PrintString(bufferSource);
    while (bufferSource[len] != '\0') ++len;
    Close(sourceFileId);

    desFileId = Open(desFile);
    PrintNum(desFile);
    PrintNum(Write(bufferSource, len, desFileId));

    Close(desFileId);
    Halt();
}
