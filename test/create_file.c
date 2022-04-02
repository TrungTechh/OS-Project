#include "syscall.h"

int main() {
    char fileName[256];
    int length;

    PrintString("Enter file name length: ");
    length = ReadNum();
    PrintString("Enter file's name: ");
    ReadString(fileName, length);

    if (CreateFile(fileName) == 0) {
        PrintString("File ");
        PrintString(fileName);
        PrintString(" created successfully!\n");
    } else
        PrintString("Create file failed\n");

    Halt();
}