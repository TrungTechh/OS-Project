#include "syscall.h"

#define Maxn 100

int main() {
    // Initialize
    int n, arr[Maxn], i, j, order;

    // Input
    do {
        PrintString("Input n - size of array (0 <= n <= 100): ");
        n = ReadNum();

        if (n >= 0 && n <= 100) break;
        PrintString("Your input is invalid!\n");
    } while(1);

    for(i = 0; i < n; i++) {
        arr[i] = ReadNum();
    }

    do {
        PrintString("Input order (0: Ascendig - 1: Descending): ");
        order = ReadNum();
        if(order == 0 || order == 1)
            break;

        PrintString("Your input is invalid!\n");
    }while(1);

    // Algorithm
    for(i = 0; i < n - 1; i++)
    {
        for(j = 0; j < n - i- 1; j++)
        {
            if(order && arr[j] < arr[j + 1]) {
                int tmp;
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
            else if(!order && arr[j] > arr[j + 1]) {
                int tmp;
                tmp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }
        
        
    // Print answer
    for(i = 0; i < n; i++) {
        PrintNum(arr[i]);
        PrintChar(' ');
    }
    PrintChar('\n');
	Halt();
}