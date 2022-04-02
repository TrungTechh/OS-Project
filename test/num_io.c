#include "syscall.h"

int main() { 
	int num;
	PrintString("Enter number\n");
	num = ReadNum();
	PrintString("Your number: ");
	PrintNum(num); 
	PrintString("\n");
	Halt();
}