#include "syscall.h"

int main() {
    PrintString("Our team:\n");
    PrintString("20127655 Tran Quoc Trung\n");
    PrintString("20127597 Bui Tan Phuong\n");
    PrintString("20127525 Dinh Nguyen Duy Khang\n\n");
    PrintString(
        "ASCII: at directory code, run build.linux/nachos -x test/ascii to "
		"print the ASCII table\n");
		
	PrintString(
        "Create File: at directory code, run build.linux/nachos -x test/create_file to "
		"create a file\n");
	PrintString("\t- Enter your file name length first\n");
	PrintString("\t- Then enter your file name you want to create\n");
    
   
    PrintString(
        "Sort: at directodry code, run build.linux/nachos -x test/bubblesort to "
        "start the sort program\n");
    PrintString("\t- Enter n (the length of the array, 0 <= n <= 100)\n");
    PrintString("\t- Enter n elements of the array\n");
    PrintString(
        "\t- Enter the order you want to sort the array with (1: increasing, "
        "2: decreasing)\n");
    PrintString(
        "\t- The program will print out the sorted array and then exit\n");
    Halt();
}
