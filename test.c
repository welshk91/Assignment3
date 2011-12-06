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

//Type INode Name Size Blocks Block-Size Depth Offset Mode UID GID Path

    printf("%s\t",
        (tflag == FTW_D) ?   "dir"   : (tflag == FTW_DNR) ? "dir(cannot read)" :
        (tflag == FTW_DP) ?  "dir(specified depth)"  : (tflag == FTW_F) ?   "file" :
        (tflag == FTW_NS) ?  "not symbolic"  : (tflag == FTW_SL) ?  "symbolic" :
        (tflag == FTW_SLN) ? "symbolic(no file)" : "???");	//Type
	
	printf("%-8ld\t",(long) sb->st_ino);			//INode
	printf("%-20s\t", fpath + ftwbuf->base);		//Name
	printf("%-10lld\t", (long long) sb->st_size);		//Size
	printf("%-5lld\t",(long long) sb->st_blocks);		//Blocks
	printf("%-5ld\t",(long) sb->st_blksize);		//Block-Size
	printf("%-6d\t",ftwbuf->level);				//Depth
	printf("%-5d\t",ftwbuf->base);				//Offset
	printf("%-5ld\t",(unsigned long) sb->st_mode);		//Mode
	printf("%-5ld\t",(long)sb->st_uid);			//UID
	printf("%-5ld\t",(long)sb->st_gid);			//GID
	printf("%s \n",fpath);					//Path

    return 0;           /* To tell nftw() to continue */
}

int
main(int argc, char *argv[])
{
    int flags = 0;
	printf("\n----------------------------------------------------------------\n");

	printf("%s\t","Type");
	printf("%-8s\t","INode");		
	printf("%-20s\t","Name");		
	printf("%-10s\t","Size(bytes)");	
	printf("%-5s\t","Blocks");		
	printf("%-5s\t","B-Size");		
	printf("%-6s\t","Depth");		
	printf("%-5s\t","Offset");		
	printf("%-5s\t","Mode");		
	printf("%-5s\t","UID");			
	printf("%-5s\t","GID");			
	printf("%s \n","Path");			


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
