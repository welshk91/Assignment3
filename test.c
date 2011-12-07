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
#include <unistd.h>


int numD=0;		//Number of Directories
int numF=0;		//Number of Files
int numDNR=0;		//Number of Directories which cannot be read
int numDP=0;		//Number of Directories of specefied depth
int numNS=0;		//Number of Non-Symbolic Links
int numSL=0;		//Number of Symbolic Links
int numSLN=0;		//Number of Symbolic Links with nonexistent files

int sizeD=0;		//Size of Directories
int sizeF=0;		//Size of Files
int sizeDNR=0;		//Size of Directories which cannot be read
int sizeDP=0;		//Size of Directories of specefied depth
int sizeNS=0;		//Size of Non-Symbolic Links
int sizeSL=0;		//Size of Symbolic Links
int sizeSLN=0;		//Size of Symbolic Links with nonexistent files

char lastMod[1024]="";	//Last Modified File
time_t lastModDate;	//Last Modified File's Date

char lastStat[1024]="";	//Last Staus Changed File
time_t lastStatDate;	//Last Status Changed File's Date

char lastAcc[1024]="";	//Last Accessed File
time_t lastAccDate;	//Last Accessed File's Date

int biggestFile=0;		//Biggest File Size
char biggestFileName[1024]="";	//Biggest File Name	
int biggestDir=0;		//Biggest Directory Size
char biggestDirName[1024]="";	//Biggest Directory Name

int depth=0;		//Depth of search

int rootFiles=0;	//Number of Root Files
int userFiles=0;	//Number of User Files
int otherFiles=0;	//Number of Files Owned by Others

long long int size[512];	//Size array, index is the depth, value is the size of depth

int numExe=0;		//Number of Executables
int numRead=0;		//Number of Readable Files
int numWrite=0;		//Number of Writable Files

static unsigned int total=0;//Total size of directory

/*Data holds directory name & it's size*/
typedef struct {
	char *name;
	int size;
} DATA;

DATA *sizeArray = NULL;	//Array of DATA that holds size of directories
int numElements = 0; 	//Number of elements in array
int numAllocated = 0; 	//Size allocated to array	

/*Adds Data to an array keeping track of directory sizes*/
int AddToArray (DATA item)
{
	if(numElements == numAllocated) { // Are more refs required?
		
		if (numAllocated == 0){
			numAllocated = 3;
		}
		else{
			numAllocated = (int)(numAllocated*1.5); //Increase allocated size 
		}

		void *_tmp = realloc(sizeArray, (numAllocated * sizeof(DATA)));
		
		//Error in reallocating
		if (!_tmp)
		{ 
			fprintf(stderr, "ERROR: Couldn't realloc memory!\n");
			return(-1); 
		}
		 
		sizeArray = (DATA*)_tmp;	
	}
	
	sizeArray[numElements] = item; 
	numElements++;
	
	return numElements;
}

/*Calculates Size of Directory*/
int sum(const char *fpath, const struct stat *sb, int typeflag) {
    total += sb->st_size;
    return 0;
}

/*Display Info*/
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
	printf("%s\t",fpath);					//Path	
	printf("\n");						
	//printf("\tLast Access: %s",ctime(&sb->st_atime));	//Last Access
	//printf("\tLast Status Change: %s",ctime(&sb->st_ctime));	//Last Status Change
	//printf("\tLast Modified: %s", ctime(&sb->st_mtime));	//Last Mod Change

	/*Add Up Types*/
	switch(tflag){
		case FTW_D:
		numD++;
		sizeD = sizeD + (long long)sb->st_size;
		break;

		case FTW_DNR:
		numDNR++;
		sizeDNR = sizeDNR + (long long)sb->st_size;
		break;

		case FTW_DP:
		numDP++;
		sizeDP = sizeDP + (long long)sb->st_size;
		break;

		case FTW_F:
		numF++;
		sizeF = sizeF + (long long)sb->st_size;
		break;

		case FTW_SL:
		numSL++;
		sizeSL = sizeSL + (long long)sb->st_size;
		break;

		case FTW_NS:
		numNS++;
		sizeNS = sizeNS + (long long)sb->st_size;
		break;

		case FTW_SLN:
		numSLN++;
		sizeSLN = sizeSLN + (long long)sb->st_size;
		break;

		default:
		printf("Unexpected File Type\n Exiting...\n");
		exit(1);
		break;
	}//end switch

	/*Calculate Size of directory + files*/
	if(tflag==FTW_D){		
		if (!fpath || access(fpath, R_OK)) {
			return 1;
	    	}
	    	if (ftw(fpath, &sum, 1)) {
	        	perror("ftw");
        		return 2;
		}
    		//printf("Total Size of '%s': %u\n", fpath, total);

		//Constructs a value to add
		DATA temp;
		temp.name = malloc((strlen(fpath) + 1) * sizeof(char));
		strncpy(temp.name, fpath, strlen(fpath) + 1);
		temp.size = total;
		if (AddToArray(temp) == -1){
			printf("Error Adding data to array\n Exiting...\n");		
			return 1;
		}
		
		total=0;
	}

	/*Executables*/
	if(access(fpath,X_OK)){
		numExe++;
	}
	
	/*Read*/
	if(access(fpath,R_OK)){
		numRead++;
	}

	/*Write*/
	if(access(fpath,W_OK)){
		numWrite++;
	}

	/*Depth*/
	if(depth<(ftwbuf->level)){
		depth = (ftwbuf->level);
	}

	/*Size*/
	if(tflag==FTW_F){
		size[(ftwbuf->level)] = size[(ftwbuf->level)] + sb->st_size; 
	}

	/*Root Files, User Files*/
	if(tflag==FTW_F && (getuid()==(long)sb->st_uid)){
		userFiles++;
	}
	else if(tflag==FTW_F && ((long)sb->st_uid)==0){
		rootFiles++;
	}
	else if(tflag==FTW_F){
		otherFiles++;
	}

	/*Last Modified File*/
	if(tflag==FTW_F && strcmp(lastMod,"")==0){
		//Undefined
		lastModDate = sb->st_mtime;
		strncpy((char*)lastMod,fpath,sizeof(lastMod));
				
	}
	else if(tflag==FTW_F && difftime(lastModDate,sb->st_mtime)<0){
		//more recent file
		lastModDate = sb->st_mtime;
		strncpy((char*)lastMod,fpath,sizeof(lastMod));
		
	}

	/*Last Status Changed File*/
	if(tflag==FTW_F && strcmp(lastStat,"")==0){
		//Undefined
		lastStatDate = sb->st_mtime;
		strncpy((char*)lastStat,fpath,sizeof(lastStat));
				
	}
	else if(tflag==FTW_F && difftime(lastStatDate,sb->st_mtime)<0){
		//more recent file
		lastStatDate = sb->st_mtime;
		strncpy((char*)lastStat,fpath,sizeof(lastStat));
		
	}

	/*Last Accessed File*/
	if(tflag==FTW_F && strcmp(lastAcc,"")==0){
		//Undefined
		lastAccDate = sb->st_mtime;
		strncpy((char*)lastAcc,fpath,sizeof(lastAcc));
				
	}
	else if(tflag==FTW_F && difftime(lastAccDate,sb->st_mtime)<0){
		//more recent file
		lastAccDate = sb->st_mtime;
		strncpy((char*)lastAcc,fpath,sizeof(lastAcc));
		
	}

	/*Biggest File*/
	if(tflag==FTW_F && biggestFile==0){
		//Undefined
		biggestFile = sb->st_size;
		strncpy((char*)biggestFileName,fpath,sizeof(biggestFileName));				
	}
	else if(tflag==FTW_F && (sb->st_size>biggestFile)){
		//bigger file
		biggestFile = sb->st_size;
		strncpy((char*)biggestFileName,fpath,sizeof(biggestFileName));
	}

	/*Biggest Directory*/
	if(tflag==FTW_D && (ftwbuf->level>0) && biggestDir==0){
		//Undefined
		biggestDir = sb->st_size;
		strncpy((char*)biggestDirName,fpath,sizeof(biggestDirName));				
	}
	else if(tflag==FTW_D && (ftwbuf->level>0) && (sb->st_size>biggestDir)){
		//bigger file
		biggestDir = sb->st_size;
		strncpy((char*)biggestDirName,fpath,sizeof(biggestDirName));
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

	printf("Directories: %-3d (%dbytes) \n",numD, sizeD);
	printf("Files: %-3d  (%dbytes) \n", numF, sizeF);	
	printf("Directories which cannot be read: %-3d (%dbytes)\n", numDNR, sizeDNR);
	printf("Directories of specified depth: %-3d (%dbytes)\n", numDP, sizeDP);
	printf("Symbolic Links: %-3d (%dbytes)\n", numSL, sizeSL);
	printf("Non-Symbolic Links: %-3d (%dbytes)\n", numNS, sizeNS);
	printf("Symbolic Links pointing to nonexistent file: %-3d (%dbytes)\n", numSLN, sizeSLN);
	printf("Number of Executables: %-3d \n", numExe);
	printf("Number of Readables: %-3d \n", numRead);
	printf("Number of Writables: %-3d \n", numWrite);
	printf("Depth: %d\n",depth);
	printf("Number of files owned by Root: %d (%f%%)\n", rootFiles, ((rootFiles*100.0)/(numF)));
	printf("Number of files owned by User: %d (%f%%)\n", userFiles, ((userFiles*100.0)/(numF)));
	printf("Number of files owned by Others: %d (%f%%)\n", otherFiles, ((otherFiles*100.0)/(numF)));
	printf("Last Modified File: %s \t %s",lastMod,ctime(&lastModDate));
	printf("Last Accessed File: %s \t %s",lastAcc,ctime(&lastAccDate));
	printf("Last File to Change Status: %s \t %s",lastStat,ctime(&lastStatDate));
	printf("Biggest File: %s \t (%dbytes)\n",biggestFileName,biggestFile);
	printf("Biggest Directory (depth>0): %s \t (%dbytes)\n",biggestDirName,biggestDir);

	int i;
	printf("Size of Each Level of Depth (bytes):\n");
	for(i=0;i<depth+1;i++){	
		printf("Size[%d]: %-20lld \t",i,(size[i]/1));
	}
	printf("\n");

	printf("Directories\n");
	//Prints Array
	for (i = 0; i < numElements; i++)
	{
		printf("Size of %s: %d\n", sizeArray[i].name, sizeArray[i].size);
	}

	//Create Graph with GNUPlot
	FILE *pipe = popen("gnuplot -persist","w");
	fprintf(pipe, "plot [-10:10] sin(x),atan(x),cos(atan(x))\n");
	int status = pclose(pipe);
	if(status==-1){
		printf("Error Graphing. Do you have GNUplot installed?\nExiting...\n");
		exit(1);
	}
	
	//Create Graph2 with GNUPlot
	FILE *pipe2 = popen("gnuplot -persist","w");
	fprintf(pipe2, "plot [-5:5] sin(x),atan(x),cos(atan(x))\n");
	int status2 = pclose(pipe2);
	if(status2==-1){
		printf("Error Graphing. Do you have GNUplot installed?\nExiting...\n");
		exit(1);
	}
		
	//Deallocate Elements & Array
	for (i = 0; i < numElements; i++)
	{
		free(sizeArray[i].name);
	}
	free(sizeArray);	

    exit(EXIT_SUCCESS);
}
