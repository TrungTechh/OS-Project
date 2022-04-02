#include"syscall.h"

int main(){
  char c[100];
  char a[100];
  int len; 
  PrintString("Enter lenght of file name: ");
  len = ReadNum();
  PrintString("Enter file name: ");
  ReadString(a,len);

  Open(a);
  Read(c,100,2);
  PrintString(c);
  Halt();
}