/*
Kevin Welsh
File System Analysis
December 8, 2011
*/

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{

//Type Name Size Depth Offset Path %9zu\t

    printf("%s\t %-20s\t %-10jd\t %-6d\t %-5d\t %-8zu %s\n",
        (tflag == FTW_D) ?   "dir"   : (tflag == FTW_DNR) ? "dir(cannot read)" :
        (tflag == FTW_DP) ?  "dir(specified depth)"  : (tflag == FTW_F) ?   "file" :
        (tflag == FTW_NS) ?  "not symbolic"  : (tflag == FTW_SL) ?  "symbolic" :
        (tflag == FTW_SLN) ? "symbolic(no file)" : "???",
	fpath + ftwbuf->base,
	(intmax_t) sb->st_size,        
	ftwbuf->level,  
	ftwbuf->base,
	sb->st_ino,
	fpath);
    return 0;           /* To tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
    int flags = 0;
	printf("\n----------------------------------------------------------------\n");
	printf("%s\t %-20s\t %-10s\t %-6s\t %-5s\t %-8s %s \n", "Type", "Name", "Size(bytes)", "Depth", "Offset", "Inode", "Path");

   if (argc > 2 && strchr(argv[2], 'd') != NULL)
        flags |= FTW_DEPTH;
    if (argc > 2 && strchr(argv[2], 'p') != NULL)
        flags |= FTW_PHYS;

   if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags)
            == -1) {
        perror("ERROR NTFW: ");
	printf("\n---------------------------------------------------------------\n");
        exit(EXIT_FAILURE);
    }
	printf("\n--------------------------------------------------------------\n");
    exit(EXIT_SUCCESS);
}
