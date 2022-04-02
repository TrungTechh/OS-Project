#include "syscall.h"

int main() {
    char fileName[] = "ab";
    int length, id;
    int i;

    for (i = 0; i < 9; i++) {
        if ((id = Open(fileName)) != -1) {
            PrintString("File ");
            PrintString(fileName);
            PrintString(" opened successfully!\n");
            PrintString("Id: ");
            PrintNum(id);
            PrintString("\n");
            
            Close(id);
        } else
            PrintString("Open file failed\n");
    }
	Halt();
}