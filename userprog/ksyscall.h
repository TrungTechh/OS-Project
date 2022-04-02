/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"
#include "synchconsole.h"
#include <stdlib.h> 

#define STRINGLENGTH 255
#define ENDSTRING '\0'

/* A buffer to read and write number */
#define MAX_NUM_LENGTH 11
char _numberBuffer[MAX_NUM_LENGTH + 2];

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

//check TAB, space, another
char isBlank(char c) { return c == 10 || c == 11 || c == 13 || c == 9 || c == ' '; }

void readUntilBlank() {
    memset(_numberBuffer, 0, sizeof(_numberBuffer));
    char c = kernel->synchConsoleIn->GetChar();

    if (c == EOF) {
        DEBUG(dbgSys, "Unexpected end of file - number expected");
        return;
    }

    if (isBlank(c)) {
        DEBUG(dbgSys, "Unexpected white-space - number expected");
        return;
    }

    int n = 0;
    while (!(isBlank(c) || c == EOF || c == '\n')) {
      _numberBuffer[n++] = c;
      if (n > MAX_NUM_LENGTH) {
          DEBUG(dbgSys, "Number is too long");
          return;
      }
      c = kernel->synchConsoleIn->GetChar();
  }
}

bool compareNumAndString(int integer, const char *s) {
    if (integer == 0) return strcmp(s, "0") == 0;

    int len = strlen(s);

    if (integer < 0 && s[0] != '-') return false;

    if (integer < 0) s++, --len, integer = -integer;

    while (integer > 0) {
        int digit = integer % 10;

        if (s[len - 1] - '0' != digit) return false;

        --len;
        integer /= 10;
    }

    return len == 0;
}

int SysReadNum() {
    readUntilBlank();
    
    int len = strlen(_numberBuffer);
    // Read nothing -> return 0
    if (len == 0) return 0;

    // Check comment below to understand this line of code
    if (strcmp(_numberBuffer, "-2147483648") == 0) return -2147483648LL;

    bool nega = (_numberBuffer[0] == '-');
    int zeros = 0;
    bool is_leading = true;
    int num = 0;
    for (int i = nega; i < len; ++i) {
        char c = _numberBuffer[i];
        if (c == '0' && is_leading)
            ++zeros;
        else
            is_leading = false;
        if (c < '0' || c > '9') {
            DEBUG(dbgSys, "Expected number but " << _numberBuffer << " found");
            return 0;
        }
        num = num * 10 + (c - '0');
    }
    // 00            01 or -0
    if (zeros > 1 || (zeros && (num || nega))) {
        DEBUG(dbgSys, "Expected number but " << _numberBuffer << " found");
        return 0;
    }

    if (nega)
        num = -num;

    if (len <= MAX_NUM_LENGTH - 2) return num;

    if (compareNumAndString(num, _numberBuffer))
        return num;
    else{
        DEBUG(dbgSys,
              "Expected int32 number but " << _numberBuffer << " found");
        return 0;
    }

    return num;
}

void SysPrintNum(int num) {
    if (num == 0) return kernel->synchConsoleOut->PutChar('0');

    if (num == -2147483648LL) {
        kernel->synchConsoleOut->PutChar('-');
        for (int i = 0; i < 10; ++i)
            kernel->synchConsoleOut->PutChar("2147483648"[i]);
        return;
    }

    if (num < 0) {
        kernel->synchConsoleOut->PutChar('-');
        num = -num;
    }
    int n = 0;
    while (num) {
        _numberBuffer[n++] = num % 10;
        num /= 10;
    }
    for (int i = n - 1; i >= 0; --i)
        kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

char SysReadChar() { return kernel->synchConsoleIn->GetChar(); }

void SysPrintChar(char character) {
    kernel->synchConsoleOut->PutChar(character);
}

int SysRandomNum() { 
  int random;

	srand(time(NULL));

	random = rand() % 2147483647 + 1;
	return random;
}

char* SysReadString(int length) {
    char tmp;
    char* buffer = new char[length + 1];
    for (int i = 0; i < length + 1; i++) {
        tmp = SysReadChar();
        if (i < length) buffer[i] = tmp;
    }
    
    buffer[length] = ENDSTRING;

    return buffer;
}

void SysPrintString(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        kernel->synchConsoleOut->PutChar(buffer[i]);
    }
}

bool SysCreateFile(char* fileName){
    bool success;
    int fileNameLength = strlen(fileName);

    if (fileNameLength == 0) {
        DEBUG(dbgSys, "\nFile name can't be empty");
        success = false;

    } else if (fileName == NULL) {
        DEBUG(dbgSys, "\nNot enough memory in system");
        success = false;

    } else {
        DEBUG(dbgSys, "\nFile's name read successfully");
        if (!kernel->fileSystem->Create(fileName,0)) {
            DEBUG(dbgSys, "\nError creating file");
            success = false;
        } else {
            success = true;
        }
    }

    return success;
}

int SysOpen(char* fileName)
{
    int fileNameLength = strlen(fileName);

    if (fileNameLength == 0) {
        DEBUG(dbgSys, "\nFile name can't be empty");
        return -1;
    }

    else if (fileName == NULL) {
        DEBUG(dbgSys, "\nNot enough memory in system");
        return -1;
    }

    else {
        int openFileID = kernel->fileSystem->FindFreeSlot();
        DEBUG(dbgSys, "\nFile's name read successfully");
        if (openFileID == -1) {
            DEBUG(dbgSys, "\nThere are fullfill processes!");
            return -1;
        }

        else if ((kernel->fileSystem->openFile[openFileID] =
                      kernel->fileSystem->Open(fileName)) != NULL) {
            
            kernel->fileSystem->openFileName[openFileID] = (fileName);
            DEBUG(dbgSys, "\nOpen file successfully");

            return openFileID;
        }

        else {
            DEBUG(dbgSys, "\nError opening file!");
            return -1;
        }
    }
}

int SysClose(int id) { return kernel->fileSystem->Close(id); }

int SysRemove(char* fileName) {
    int fileNameLength = strlen(fileName);

    if (fileNameLength == 0) {
        DEBUG(dbgSys, "\nFile name can't be empty");
        return -1;
    }

    else if (fileName == NULL) {
        DEBUG(dbgSys, "\nNot enough memory in system");
        return -1;

    }

    else {
        if (kernel->fileSystem->isOpen(fileName)) {
            DEBUG(dbgSys, "\nFile " << fileName << " is openning!");
            return -1;
        } else {
            bool success = kernel->fileSystem->Remove(fileName);
            if (!success) {
                DEBUG(dbgSys, "\nError removing file!");
                return -1;
            } else {
                DEBUG(dbgSys, "\nRemoving file successfully!");
                return 0;
            }
        }
    }
}

int SysRead(char *buffer, int size, int id)
{
    if (id == 0) {
        return kernel->synchConsoleIn->GetString(buffer, size);
    }
    return kernel->fileSystem->Read(buffer, size, id);
}

int SysWrite(char *buffer, int size, int id)
{
    if (id == 1) {
        return kernel->synchConsoleOut->PutString(buffer, size);
    }
    return kernel->fileSystem->Write(buffer, size, id);
}

int SysSeek(int seekPos, int fileId) {
    if (fileId <= 1) {
        DEBUG(dbgSys, "\nCan't seek in console");
        return -1;
    }
    return kernel->fileSystem->Seek(seekPos, fileId);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
