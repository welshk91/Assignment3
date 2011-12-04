#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>

/* 
* example for
* int ftw(const char *path, int (*fn)(const char *,
* const struct stat *ptr, int flag, int ndirs);
*/

int ffn(const char *path, const struct stat *ptr, int flag)
{
printf("Found\n path:%s\nflag%d\n", path, flag);
return 0;
}

int main(int argc, char ** argv)
{
char * inipath; 
// example: 
if (argv[1] && *argv[1])
{ 
inipath = argv[1];
}
else
{
fprintf(stderr,"usage: exftw PATH\n");
exit(-1);
}

if(ftw(inipath,ffn,20) != 0)
{
perror("ftw");exit(2);
}

return 0;
}
