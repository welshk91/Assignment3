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
#include <time.h>

int numD=0;		//Number of Directories
int numF=0;		//Number of Files
int numDNR=0;		//Number of Directories which cannot be read
int numDP=0;		//Number of Directories of specefied depth
int numNS=0;		//Number of Non-Symbolic Links
int numSL=0;		//Number of Symbolic Links
int numSLN=0;		//Number of Symbolic Links with nonexistent files

char lastMod[1024]="";	//Last Modified File
time_t lastModDate;	//Last Modified File's Date

char lastStat[1024]="";	//Last Staus Changed File
time_t lastStatDate;	//Last Status Changed File's Date

char lastAcc[1024]="";	//Last Accessed File
time_t lastAccDate;	//Last Accessed File's Date


static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{


	printf("%s\t",
        (tflag == FTW_D) ?   "dir"   : (tflag == FTW_DNR) ? "dir(cannot read)" :
        (tflag == FTW_DP) ?  "dir(specified depth)"  : (tflag == FTW_F) ?   "file" :
        (tflag == FTW_NS) ?  "not symbolic"  : (tflag == FTW_SL) ?  "symbolic" :
        (tflag == FTW_SLN) ? "symbolic(no file)" : "???");	//Type
	
	printf("%-8ld\t",(long) sb->st_ino);			//INode
	printf("%-20s\t", fpath + ftwbuf->base);		//Name
	printf("%-8lld\t", (long long) sb->st_size);		//Size
	printf("%-5lld\t",(long long) sb->st_blocks);		//Blocks
	printf("%-5ld\t",(long) sb->st_blksize);		//Block-Size
	printf("%-4d\t",ftwbuf->level);				//Depth
	printf("%-4d\t",ftwbuf->base);				//Offset
	printf("%-5ld\t",(unsigned long) sb->st_mode);		//Mode
	printf("%-5ld\t",(long)sb->st_uid);			//UID
	printf("%-5ld\t",(long)sb->st_gid);			//GID
	printf("%s",fpath);					//Path	
	printf("\n");						
	//printf("\tLast Access: %s",ctime(&sb->st_atime));	//Last Access
	//printf("\tLast Status Change: %s",ctime(&sb->st_ctime));	//Last Status Change
	//printf("\tLast Modified: %s", ctime(&sb->st_mtime));	//Last Mod Change

	/*Add Up Types*/
	switch(tflag){
		case FTW_D:
		numD++;
		break;

		case FTW_DNR:
		numDNR++;
		break;

		case FTW_DP:
		numDP++;
		break;

		case FTW_F:
		numF++;
		break;

		case FTW_SL:
		numSL++;
		break;

		case FTW_NS:
		numNS++;
		break;

		case FTW_SLN:
		numSLN++;
		break;

		default:
		printf("Unexpected File Type\n Exiting...\n");
		exit(1);
		break;
	}//end switch

	/*Last Modified File*/
	if(tflag==FTW_F && strcmp(lastMod,"")==0){
		//Undefined
		//printf("undefined\n");
		lastModDate = sb->st_mtime;
		strncpy((char*)lastMod,fpath,sizeof(lastMod));
				
	}
	else if(tflag==FTW_F && difftime(lastModDate,sb->st_mtime)<0){
		//more recent file
		//printf("more recent\n");
		lastModDate = sb->st_mtime;
		strncpy((char*)lastMod,fpath,sizeof(lastMod));
		
	}

	/*Last Status Changed File*/
	if(tflag==FTW_F && strcmp(lastStat,"")==0){
		//Undefined
		//printf("undefined\n");
		lastStatDate = sb->st_mtime;
		strncpy((char*)lastStat,fpath,sizeof(lastStat));
				
	}
	else if(tflag==FTW_F && difftime(lastStatDate,sb->st_mtime)<0){
		//more recent file
		//printf("more recent\n");
		lastStatDate = sb->st_mtime;
		strncpy((char*)lastStat,fpath,sizeof(lastStat));
		
	}

	/*Last Accessed File*/
	if(tflag==FTW_F && strcmp(lastAcc,"")==0){
		//Undefined
		//printf("undefined\n");
		lastAccDate = sb->st_mtime;
		strncpy((char*)lastAcc,fpath,sizeof(lastAcc));
				
	}
	else if(tflag==FTW_F && difftime(lastAccDate,sb->st_mtime)<0){
		//more recent file
		//printf("more recent\n");
		lastAccDate = sb->st_mtime;
		strncpy((char*)lastAcc,fpath,sizeof(lastAcc));
		
	}



    return 0;           /* To tell nftw() to continue */
}//end display_info

int
main(int argc, char *argv[])
{
    int flags = 0;
	printf("\n----------------------------------------------------------------\n");

	printf("%s\t","Type");
	printf("%-8s\t","INode");		
	printf("%-20s\t","Name");		
	printf("%-8s\t","Size(bytes)");	
	printf("%-5s\t","Block");		
	printf("%-5s\t","B-Size");		
	printf("%-4s\t","Depth");		
	printf("%-4s\t","Offset");		
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
	
	printf("\nStatistics\n");	

	printf("Number of Directories: %d \n",numD);
	printf("Number of Files: %d \n", numF);	
	printf("Number of Directories which cannot be read: %d \n", numDNR);
	printf("Number of Directories of specified depth: %d \n", numDP);
	printf("Number of Symbolic Links: %d \n", numSL);
	printf("Number of Non-Symbolic Links: %d \n", numNS);
	printf("Number of Symbolic Links pointing to nonexistent file: %d \n", numSLN);
	printf("Last Modified File: %s \t %s",lastMod,ctime(&lastModDate));
	printf("Last Accessed File: %s \t %s",lastAcc,ctime(&lastAccDate));
	printf("Last File to Change Status: %s \t %s",lastStat,ctime(&lastStatDate));


    exit(EXIT_SUCCESS);
}
