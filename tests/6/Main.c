/*
 * 不允许的系统调用 write file
 */

#include <stdio.h>
#include <stdlib.h>

int main()
{
   FILE * fp;

   fp = fopen ("/dev/null", "w");
   fprintf(fp, "test");
   fclose(fp);
   printf("write file succeesed");
   return 0;
}