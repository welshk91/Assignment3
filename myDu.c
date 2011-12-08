/*
Kevin Welsh
File System Analysis
December 8, 2011
*/

/*
Dependencies: GNUPlot for graphing
Compile: gcc -Wall -g myDu.c
Usage: ./a.out [filepath] [depth]
Filepath is optional, defaults to /
Depth is optional
You can find more output in the files display_info.dat & directories.dat
*/

#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

int numD=0;			//Number of Directories
int numF=0;			//Number of Files
int numDNR=0;			//Number of Directories which cannot be read
int numDP=0;			//Number of Directories of specefied depth
int numNS=0;			//Number of Non-Symbolic Links
int numSL=0;			//Number of Symbolic Links
int numSLN=0;			//Number of Symbolic Links with nonexistent files

long long int sizeD=0;		//Size of Directories
long long int sizeF=0;		//Size of Files
long long int sizeDNR=0;	//Size of Directories which cannot be read
long long int sizeDP=0;		//Size of Directories of specefied depth
long long int sizeNS=0;		//Size of Non-Symbolic Links
long long int sizeSL=0;		//Size of Symbolic Links
long long int sizeSLN=0;	//Size of Symbolic Links with nonexistent files

long long int sizeRoot=0;	//Size Root takes up
long long int sizeUser=0;	//Size User takes up
long long int sizeOther=0;	//Size Others take up

char lastMod[1024]="";		//Last Modified File
time_t lastModDate;		//Last Modified File's Date

char lastStat[1024]="";		//Last Staus Changed File
time_t lastStatDate;		//Last Status Changed File's Date

char lastAcc[1024]="";		//Last Accessed File
time_t lastAccDate;		//Last Accessed File's Date

long long int biggestFile=0;	//Biggest File Size
char biggestFileName[1024]="";	//Biggest File Name	
long long int biggestDir=0;	//Biggest Directory Size
char biggestDirName[1024]="";	//Biggest Directory Name

int depth=0;			//Depth of search

int rootFiles=0;		//Number of Root Files
int userFiles=0;		//Number of User Files
int otherFiles=0;		//Number of Files Owned by Others

FILE *file;			//FILE pointer to handle files for logging and graphs

long long int size[512];	//Size array, index is the depth, value is the size of depth

static unsigned long long int total=0;	//Total size of directory

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
	//Need More Space
	if(numElements == numAllocated) {
		
		if (numAllocated == 0){
			numAllocated = 20;
		}
		else{
			//Increase Allocated Size
			numAllocated = (int)(numAllocated*1.2);
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
    total += (long long)sb->st_size;
    return 0;
}

/*Display Info*/
static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
	//Prints TYPE
	file = fopen("display_info.dat","a+");
	fprintf(file,"%s\t",
        (tflag == FTW_D) ?   "dir"   : (tflag == FTW_DNR) ? "dir(cannot read)" :
        (tflag == FTW_DP) ?  "dir(specified depth)"  : (tflag == FTW_F) ?   "file" :
        (tflag == FTW_NS) ?  "not symbolic"  : (tflag == FTW_SL) ?  "symbolic" :
        (tflag == FTW_SLN) ? "symbolic(no file)" : "???");
	
	printf("%s\t",
        (tflag == FTW_D) ?   "dir"   : (tflag == FTW_DNR) ? "dir(cannot read)" :
        (tflag == FTW_DP) ?  "dir(specified depth)"  : (tflag == FTW_F) ?   "file" :
        (tflag == FTW_NS) ?  "not symbolic"  : (tflag == FTW_SL) ?  "symbolic" :
        (tflag == FTW_SLN) ? "symbolic(no file)" : "???");

	//Prints INode
	fprintf(file,"%-8ld\t",(long) sb->st_ino);
	printf("%-8ld\t",(long) sb->st_ino);

	//Prints Name
	fprintf(file,"%-20s\t", fpath + ftwbuf->base);
	printf("%-20s\t", fpath + ftwbuf->base);

	//Prints Size
	fprintf(file,"%-8lld\t", (long long) sb->st_size);
	printf("%-8lld\t", (long long) sb->st_size);		

	//Prints Blocks
	fprintf(file,"%-5lld\t",(long long) sb->st_blocks);
	printf("%-5lld\t",(long long) sb->st_blocks);		
	
	//Prints Block-Size
	fprintf(file,"%-5ld\t",(long) sb->st_blksize);
	printf("%-5ld\t",(long) sb->st_blksize);		

	//Prints Depth
	fprintf(file,"%-4d\t",ftwbuf->level);
	printf("%-4d\t",ftwbuf->level);				

	//Prints Offset
	fprintf(file,"%-4d\t",ftwbuf->base);
	printf("%-4d\t",ftwbuf->base);				

	//Prints Mode
	fprintf(file,"%-5ld\t",(unsigned long) sb->st_mode);
	printf("%-5ld\t",(unsigned long) sb->st_mode);		

	//Prints UID
	fprintf(file,"%-5ld\t",(long)sb->st_uid);
	printf("%-5ld\t",(long)sb->st_uid);			

	//Prints GID
	fprintf(file,"%-5ld\t",(long)sb->st_gid);
	printf("%-5ld\t",(long)sb->st_gid);			

	//Prints Path
	fprintf(file,"%s\t",fpath);
	printf("%s\t",fpath);				
		
	fprintf(file,"\n");
	printf("\n");
						
	//printf("\tLast Access: %s",ctime(&sb->st_atime));		//Last Access
	//printf("\tLast Status Change: %s",ctime(&sb->st_ctime));	//Last Status Change
	//printf("\tLast Modified: %s", ctime(&sb->st_mtime));		//Last Mod Change

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
 
	/*Calculate Size of directory + files, AVOID DEV DIRECTORY*/
	//if(tflag==FTW_D || !strncmp(fpath,"/dev",4)){		
	if(tflag==FTW_D){	
		if (!fpath || access(fpath, R_OK)) {
			return 1;
	    	}
	    	if (ftw(fpath, &sum, 1)) {
	        	perror("ftw");
        		return 2;
		}

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

	/*Determines Biggest Depth*/
	if(depth<(ftwbuf->level)){
		depth = (ftwbuf->level);
	}

	/*Size Based on each level of depth*/
	if(tflag==FTW_F){
		size[(ftwbuf->level)] = size[(ftwbuf->level)] + (long long)sb->st_size; 
	}

	/*Root Files, User Files, Other Files (excludes directories)*/
	if(tflag==FTW_F && (getuid()==(long)sb->st_uid)){
		userFiles++;
		sizeUser = sizeUser + (long long)sb->st_size;		
	}
	else if(tflag==FTW_F && ((long)sb->st_uid)==0){
		rootFiles++;
		sizeRoot = sizeRoot + (long long)sb->st_size;
	}
	else if(tflag==FTW_F){
		otherFiles++;
		sizeOther = sizeOther + (long long)sb->st_size;
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
		lastStatDate = sb->st_ctime;
		strncpy((char*)lastStat,fpath,sizeof(lastStat));
				
	}
	else if(tflag==FTW_F && difftime(lastStatDate,sb->st_mtime)<0){
		//more recent file
		lastStatDate = sb->st_ctime;
		strncpy((char*)lastStat,fpath,sizeof(lastStat));
		
	}

	/*Last Accessed File*/
	if(tflag==FTW_F && strcmp(lastAcc,"")==0){
		//Undefined
		lastAccDate = sb->st_atime;
		strncpy((char*)lastAcc,fpath,sizeof(lastAcc));
				
	}
	else if(tflag==FTW_F && difftime(lastAccDate,sb->st_mtime)<0){
		//more recent file
		lastAccDate = sb->st_atime;
		strncpy((char*)lastAcc,fpath,sizeof(lastAcc));
		
	}

	/*Biggest File*/
	if(tflag==FTW_F && biggestFile==0){
		//Undefined
		biggestFile = (long long)sb->st_size;
		strncpy((char*)biggestFileName,fpath,sizeof(biggestFileName));				
	}
	else if(tflag==FTW_F && (sb->st_size>biggestFile)){
		//bigger file
		biggestFile = (long long)sb->st_size;
		strncpy((char*)biggestFileName,fpath,sizeof(biggestFileName));
	}

	/*Biggest Directory*/
	if(tflag==FTW_D && (ftwbuf->level>0) && biggestDir==0){
		//Undefined
		biggestDir = (long long)sb->st_size;
		strncpy((char*)biggestDirName,fpath,sizeof(biggestDirName));				
	}
	else if(tflag==FTW_D && (ftwbuf->level>0) && (sb->st_size>biggestDir)){
		//bigger file
		biggestDir = (long long)sb->st_size;
		strncpy((char*)biggestDirName,fpath,sizeof(biggestDirName));
	}

	fclose(file);

    return 0;

}//end display_info

int
main(int argc, char *argv[])
{
    int flags = 0;
	printf("\n----------------------------------------------------------------\n");

	file = fopen("display_info.dat","w+");
	
	fprintf(file,"%s\t","Type");
	printf("%s\t","Type");
	fprintf(file,"%-8s\t","INode");
	printf("%-8s\t","INode");
	fprintf(file,"%-20s\t","Name");		
	printf("%-20s\t","Name");		
	fprintf(file,"%-8s\t","Size(bytes)");
	printf("%-8s\t","Size(bytes)");	
	fprintf(file,"%-5s\t","Block");
	printf("%-5s\t","Block");		
	fprintf(file,"%-5s\t","B-Size");
	printf("%-5s\t","B-Size");		
	fprintf(file,"%-4s\t","Depth");
	printf("%-4s\t","Depth");		
	fprintf(file,"%-4s\t","Offset");
	printf("%-4s\t","Offset");		
	fprintf(file,"%-5s\t","Mode");
	printf("%-5s\t","Mode");		
	fprintf(file,"%-5s\t","UID");
	printf("%-5s\t","UID");			
	fprintf(file,"%-5s\t","GID");
	printf("%-5s\t","GID");
	fprintf(file,"%s\n","Path");
	printf("%s \n","Path");	

	fclose(file);		

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

	printf("Directories: %-3d (%lldbytes) \t (%f KB) \t (%f MB) \t (%f GB)\n",numD, sizeD, sizeD/1024.0, sizeD/1048576.0, sizeD/1073741824.0);
	printf("Files: %-3d  (%lldbytes) \t (%f KB) \t (%f MB) \t (%f GB)\n", numF, sizeF, sizeF/1024.0, sizeF/1048576.0, sizeF/1073741824.0);	
	printf("Directories (cannot be read): %-3d (%lldbytes) \t (%f KB) \t (%f MB)\n", numDNR, sizeDNR, sizeDNR/1024.0, sizeDNR/1048576.0);
	printf("Directories (specified depth): %-3d (%lldbytes) \t (%f KB) \t (%f MB)\n", numDP, sizeDP, sizeDP/1024.0, sizeDP/1048576.0);
	printf("Symbolic Links: %-3d (%lldbytes) \t (%f KB) \t (%f MB)\n", numSL, sizeSL, sizeSL/1024.0, sizeSL/1048576.0);
	printf("Non-Symbolic Links: %-3d (%lldbytes) \t (%f KB) \t (%f MB)\n", numNS, sizeNS, sizeNS/1024.0, sizeNS/1048576.0);
	printf("Symbolic Links (nonexistent file): %-3d (%lldbytes) \t (%f KB) \t (%f MB)\n\n", numSLN, sizeSLN, sizeSLN/1024.0, sizeSLN/1048576.0);
	printf("Depth: %d\n",depth);
	printf("Number of files owned by Root: %d (%f%%)\n", rootFiles, ((rootFiles*100.0)/(numF)));
	printf("Number of files owned by User: %d (%f%%)\n", userFiles, ((userFiles*100.0)/(numF)));
	printf("Number of files owned by Others: %d (%f%%)\n\n", otherFiles, ((otherFiles*100.0)/(numF)));
	printf("Last Modified File: %s \t %s",lastMod,ctime(&lastModDate));
	printf("Last Accessed File: %s \t %s",lastAcc,ctime(&lastAccDate));
	printf("Last File to Change Status: %s \t %s\n",lastStat,ctime(&lastStatDate));
	printf("Biggest File: %s \t (%lldbytes) \t (%f KB) \t (%f MB) \t (%f GB)\n",biggestFileName,biggestFile, biggestFile/1024.0, biggestFile/1048576.0,biggestFile/1073741824.0);
	printf("Biggest Directory (depth>0): %s \t (%lldbytes) \t (%f KB) \t (%f MB) \t (%f GB)\n\n",biggestDirName,biggestDir, biggestDir/1024.0, biggestDir/1048576.0,biggestDir/1073741824.0);
	printf("Size of Root: %lld bytes \t %f KB \t %f MB \t %f GB\n", sizeRoot,sizeRoot/1024.0, sizeRoot/1048576.0, sizeRoot/1073741824.0);
	printf("Size of User: %lld bytes \t %f KB \t %f MB \t %f GB\n", sizeUser,sizeUser/1024.0, sizeUser/1048576.0, sizeUser/1073741824.0);
	printf("Size of Other:%lld bytes \t %f KB \t %f MB \t %f GB\n", sizeOther,sizeOther/1024.0, sizeOther/1048576.0, sizeOther/1073741824.0);
	printf("Total Size:   %lld bytes \t %f KB \t %f MB \t %f GB\n", (sizeRoot+sizeUser+sizeOther), (sizeRoot+sizeUser+sizeOther)/1024.0, (sizeRoot+sizeUser+sizeOther)/1048576.0,(sizeRoot+sizeUser+sizeOther)/1073741824.0);

	int i;
/*	//Print Size at each depth-level
	printf("Size of Each Level of Depth (bytes):\n");
	for(i=0;i<depth+1;i++){	
		printf("Size[%d]: %-20lld \t",i,(size[i]/1));
	}
	printf("\n");

*/

	//Prints Array in Directories.dat file
	file = fopen("directories.dat","w+");
	fprintf(file,"Directories\n");
	fclose(file);
	for (i = 0; i < numElements; i++)
	{
		file = fopen("directories.dat","a+");
		fprintf(file,"Size of %s: %d\n", sizeArray[i].name, sizeArray[i].size);
		fclose(file);
	}

	//Set-Up Data for Files Graph
	file = fopen("output1.dat","w+");
	fprintf(file,"User\tRoot\tOther\n");
	fprintf(file,"%d\t0\t0\n", userFiles);
	fprintf(file,"0\t%d\t0\n", rootFiles);
	fprintf(file,"0\t0\t%d\n", otherFiles);
	fclose(file);

	//Set-Up Data for Size Graph
	file = fopen("output2.dat","w+");
	fprintf(file,"User\tRoot\tOther\n");
	fprintf(file,"%f\t0\t0\n", sizeUser/1024.0);
	fprintf(file,"0\t%f\t0\n", sizeRoot/1024.0);
	fprintf(file,"0\t0\t%f\n", sizeOther/1024.0);
	fclose(file);

	//Create Files Graph with GNUPlot
	FILE *pipe = popen("gnuplot -persist","w");
	fprintf(pipe, "set terminal postscript color solid\n");	
	//fprintf(pipe, "set terminal png\n");	
	fprintf(pipe, "set size 1,1\n");
	fprintf(pipe, "set title 'Number of Files per User' font 'Utopia,28'\n");
	fprintf(pipe, "set xlabel 'Users' font 'Utopia,18'\n");
	fprintf(pipe, "set ylabel 'Files' font 'Utopia,18'\n");
	fprintf(pipe, "set boxwidth 0.9\n");
	fprintf(pipe, "set border 3\n");
	fprintf(pipe, "set xtic nomirror\n");
	fprintf(pipe, "set noxtic \n");
	fprintf(pipe, "set ytic nomirror\n");
	fprintf(pipe, "set style data histogram\n");
	fprintf(pipe, "set xrange [-1:3]\n");
	fprintf(pipe, "set style histogram cluster gap 1\n");
	fprintf(pipe, "set style fill solid border -1\n");
	fprintf(pipe, "set output 'graph_files.ps'\n");
	fprintf(pipe, "plot 'output1.dat' using 1 with boxes title col, 'output1.dat' using 2 with boxes title col, 'output1.dat' using 3 with boxes title col\n");

	int status = pclose(pipe);
	if(status==-1){
		printf("Error Graphing. Do you have GNUplot installed?\nExiting...\n");
		exit(1);
	}

	//Create Size Graph
	pipe = popen("gnuplot -persist","w");
	fprintf(pipe, "set terminal postscript color solid\n");	
	//fprintf(pipe, "set terminal png\n");
	fprintf(pipe, "set size 1,1\n");
	fprintf(pipe, "set title 'Disk Usage per User' font 'Utopia,28'\n");
	fprintf(pipe, "set xlabel 'Users' font 'Utopia,18'\n");
	fprintf(pipe, "set ylabel 'Size (Kb)' font 'Utopia,18'\n");
	fprintf(pipe, "set boxwidth 0.9\n");
	fprintf(pipe, "set border 3\n");
	fprintf(pipe, "set xtic nomirror\n");
	fprintf(pipe, "set noxtic \n");
	fprintf(pipe, "set ytic nomirror\n");
	fprintf(pipe, "set style data histogram\n");
	fprintf(pipe, "set xrange [-1:3]\n");
	fprintf(pipe, "set style histogram cluster gap 1\n");
	fprintf(pipe, "set style fill solid border -1\n");
	fprintf(pipe, "set output 'graph_size.ps'\n");
	fprintf(pipe, "plot 'output2.dat' using 1 with boxes title col, 'output2.dat' using 2 with boxes title col, 'output2.dat' using 3 with boxes title col\n");

	status = pclose(pipe);
	if(status==-1){
		printf("Error Graphing. Do you have GNUplot installed?\nExiting...\n");
		exit(1);
	}
			
	//Deallocate Elements & Array
	for (i = 0; i < numElements; i++)
	{
		free(sizeArray[i].name);
	}
	free(sizeArray);	

	//Delete files used for graphs
	if (remove("output1.dat") == -1){
  		perror("Error deleting output1.dat");
	}
	if (remove("output2.dat") == -1){
  		perror("Error deleting output1.dat");
	}


    exit(EXIT_SUCCESS);
}
