#include "syscall.h"

int main() {
    int i;
    PrintString("This is ascii table; if output is not appear, it can be blank characters or not-printed charaters\n");
    for (i = 32; i <= 126; i++) {
        PrintNum(i);
        PrintChar(' ');
        PrintChar((char)i);
        PrintChar('\n');
    }
    Halt();
}