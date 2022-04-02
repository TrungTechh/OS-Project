// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

#define MaxFileLength 32

 char* stringUser2System(int addr, int convert_length = -1) {
    int length = 0;
    bool stop = false;
    char* str;

    do {
        int oneChar;
        kernel->machine->ReadMem(addr + length, 1, &oneChar);
        length++;
        // if convert_length == -1, we use '\0' to terminate the process
        // otherwise, we use convert_length to terminate the process
        stop = ((oneChar == '\0' && convert_length == -1) ||
                length == convert_length);
    } while (!stop);

    str = new char[length];
    for (int i = 0; i < length; i++) {
        int oneChar;
        kernel->machine->ReadMem(addr + i, 1,
                                 &oneChar);  // copy characters to kernel space
        str[i] = (unsigned char)oneChar;
    }
    return str;
}

void StringSys2User(char* str, int addr, int convert_length = -1) {
    int length = (convert_length == -1 ? strlen(str) : convert_length);
    for (int i = 0; i < length; i++) {
        kernel->machine->WriteMem(addr + i, 1,
                                  str[i]);  // copy characters to user space
    }
    kernel->machine->WriteMem(addr + length, 1, '\0');
}

void increaseProgramCounter(){
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	
	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			
}

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    //DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
		case NoException:  // return control to kernel
				kernel->interrupt->setStatus(SystemMode);
				DEBUG(dbgSys, "Switch to system mode\n");
				break;
		case PageFaultException:
		case ReadOnlyException:
		case BusErrorException:
		case AddressErrorException:
		case OverflowException:
		case IllegalInstrException:
		case NumExceptionTypes:
			cerr << "Error " << which << " occurs\n";
			SysHalt();
			ASSERTNOTREACHED();
		
		case SyscallException:
			switch(type) {
				case SC_Halt:
					DEBUG('a', "\nShutdown, initiated by user program. ");
					printf("\nShutdown, initiated by user program. ");
					SysHalt();
					ASSERTNOTREACHED();
					return;

				case SC_Add:
				{
					DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

					/* Process SysAdd Systemcall*/
					int result;
					result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
							/* int op2 */(int)kernel->machine->ReadRegister(5));

					DEBUG(dbgSys, "Add returning with " << result << "\n");
					/* Prepare Result */
					kernel->machine->WriteRegister(2, (int)result);
					return ;
				}

				case SC_ReadNum:
				{
					int result = SysReadNum();
					kernel->machine->WriteRegister(2, result);
					return increaseProgramCounter();
				}
					
                case SC_PrintNum:
				{
					int character = kernel->machine->ReadRegister(4);
                    SysPrintNum(character);
                    return increaseProgramCounter();
				}
					
				case SC_ReadChar:{
					char res = SysReadChar();
					kernel->machine->WriteRegister(2, (int)res);
					return increaseProgramCounter();	
				}

				case SC_PrintChar:
				{
					char character = (char)kernel->machine->ReadRegister(4);
					SysPrintChar(character);
					return increaseProgramCounter();
				}
					
				case SC_RandomNum:
				{
					int result = SysRandomNum();
					kernel->machine->WriteRegister(2, (int)result);
					DEBUG(dbgSys, "Random number is " << result << "\n");
					return increaseProgramCounter();
				}

				case SC_ReadString:
                {
					int addr = kernel->machine->ReadRegister(4);
                    int length = kernel->machine->ReadRegister(5);

                    if (length > 10) {
                        DEBUG(dbgSys, "String's length exceeds max length!");
                        SysHalt();
                    }

                    char* buffer = stringUser2System(addr);
					buffer=SysReadString(length);
                    StringSys2User(buffer, addr, length);
                    delete[] buffer;
                    buffer = NULL;
                    return increaseProgramCounter();
				}
	
				case SC_PrintString:
				{
					int memPtr = kernel->machine->ReadRegister(4);  // read address of C-string
					char* buffer = stringUser2System(memPtr);
					SysPrintString(buffer, strlen(buffer));
					delete[] buffer;
					return increaseProgramCounter();
				}

				case SC_CreateFile:
                {
					int virtAddr = kernel->machine->ReadRegister(4);
                    char* fileName = stringUser2System(virtAddr);

                    if (SysCreateFile(fileName))
                        kernel->machine->WriteRegister(2, 0);
                    else
                        kernel->machine->WriteRegister(2, -1);

                    delete[] fileName;
                    return increaseProgramCounter();
				}
				
				//Open for write != open for read
				case SC_Open:
				{
					int virtAddr;
					char* fileName;

					virtAddr = kernel->machine->ReadRegister(4);
					DEBUG(dbgSys, "\nReading filename.");

					fileName = stringUser2System(virtAddr);
					kernel->machine->WriteRegister(2, SysOpen(fileName));

					return increaseProgramCounter();
				}

				case SC_Close:
				{
					DEBUG(dbgSys, "Reading fileID.");
					int id = kernel->machine->ReadRegister(4);
					kernel->machine->WriteRegister(2, SysClose(id));

					return increaseProgramCounter();
				}

				case SC_Remove: {
                    int virtAddr;
                    char* filename;

                    virtAddr = kernel->machine->ReadRegister(4);

                    DEBUG(dbgSys, "Reading filename.");
                    filename = stringUser2System(virtAddr);

                    kernel->machine->WriteRegister(2, SysRemove(filename));

                    delete filename;
                    return increaseProgramCounter();
                }

				case SC_Read:
				{
					int virtAddr = kernel->machine->ReadRegister(4);
					int charCount = kernel->machine->ReadRegister(5);
					int fileId = kernel->machine->ReadRegister(6);
					
					char* buffer = stringUser2System(virtAddr, charCount);
					DEBUG(dbgFile,"Read " << charCount << " chars from file " << fileId << "\n");

					kernel->machine->WriteRegister(2, SysRead(buffer, charCount, fileId));
					StringSys2User(buffer, virtAddr, charCount);

    				delete[] buffer;
                    return increaseProgramCounter();
				}

                case SC_Write:
                {
					int virtAddr = kernel->machine->ReadRegister(4);
					int charCount = kernel->machine->ReadRegister(5);
					int fileId = kernel->machine->ReadRegister(6);
					char* buffer = stringUser2System(virtAddr, charCount);

					DEBUG(dbgFile,"Write " << charCount << " chars to file " << fileId << "\n");

					kernel->machine->WriteRegister(2, SysWrite(buffer, charCount, fileId));
					StringSys2User(buffer, virtAddr, charCount);

					delete[] buffer;
                    return increaseProgramCounter();
				}
				
				case SC_Seek:
                {
					int seekPos = kernel->machine->ReadRegister(4);
					int fileId = kernel->machine->ReadRegister(5);

					kernel->machine->WriteRegister(2, SysSeek(seekPos, fileId));

					return increaseProgramCounter();
				}

				default:
					cerr << "Unexpected system call " << type << "\n";
					break;
			}
			break;
		default:
			cerr << "Unexpected user mode exception" << (int)which << "\n";
			break;
    }
    ASSERTNOTREACHED();

}
