#include "syscall.h"

#define MODE_READWRITE 0
#define MODE_READ 1

#define stdin 0
#define stdout 1

int main() {
    char buffer[100];
    int i;
    int write;

    // int fileid = Open("abc");
    // int read = Read(buffer, 50, fileid);
    // int len = 0;
    // while (buffer[len] != '\0') ++len;
    // PrintString("Read ");
    // PrintNum(len);
    // PrintString(" characters: ");
    // PrintString(buffer);
    // PrintString("\n");
    // Close(fileid);

    // fileid = Open("12");
    // write = Write(buffer, len, fileid);

    // PrintString("Write ");
    // PrintNum(write);
    // PrintString(" characters: ");
    // PrintString(buffer);
    // PrintString("\n");
    // Close(fileid);

    // fileid = Open("abc2.txt");
    // read = Read(buffer, 50, fileid);
    // len = 0;
    // while (buffer[len] != '\0') ++len;

    // PrintString("Read ");
    // PrintNum(len);
    // PrintString(" characters: ");
    // PrintString(buffer);
    // PrintString("\n");
    // // Write to the same file
    // write = Write(buffer, len, fileid);
    // PrintString("Write ");
    // PrintNum(write);
    // PrintString(" characters: ");
    // PrintString(buffer);
    // PrintString("\n");
    // Close(fileid);

	// int fileid = Open("abc");
    // int fileid2 = Open("12");

    // int read = Read(buffer, 50, fileid);
    // int len = 0;
    // while (buffer[len] != '\0') ++len;
    // // PrintString("Type a string (use ctrl+D to end typing):\n");
	// // Read(buffer, 50, stdin);
    // // len = 0;
    // // while (buffer[len] != '\0') ++len;
    // PrintNum(Write(buffer, len, fileid2));
    // Close(fileid);
    // Close(fileid2);
    ReadString(buffer,50);
    PrintString(buffer);
	Halt();
}