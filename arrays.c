#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *name;
	int size;
} DATA;

DATA 	*sizeArray = NULL;	//Array of DATA that holds size of directories
int 	numElements = 0; 	//Number of elements in array
int	numAllocated = 0; 	//Size allocated to array
const char* fpath = "/home/kev/Programs/abc";

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

int main()
{
	int i;

	//Constructs a value to add
	DATA temp;
	temp.name = malloc((strlen(fpath) + 1) * sizeof(char));
	strncpy(temp.name, fpath, strlen(fpath) + 1);
	temp.size = 86;
	if (AddToArray(temp) == -1){
		printf("Error Adding data to array\n Exiting...\n");		
		return 1;
	}
		
	//Prints Array
	for (i = 0; i < numElements; i++)
	{
		printf("Size of %s: %d\n", sizeArray[i].name, sizeArray[i].size);
	}
	
	//Deallocate Elements & Array
	for (i = 0; i < numElements; i++)
	{
		free(sizeArray[i].name);
	}
	free(sizeArray);	
	
	return 0;
}
