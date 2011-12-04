#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <dirent.h>

void listdir(char* dirname, int lvl);

int main(int argc, char** argv)
{

  if (argc != 2) {
    fprintf(stderr, "Incorrect usage!\n");
    exit(-1);
  }
  listdir(argv[1], 0);


  return 0;
}

void listdir(char* dirname, int lvl)
{

  int i;
  DIR* d_fh;
  struct dirent* entry;
  char longest_name[4096];

  while( (d_fh = opendir(dirname)) == NULL) {
    fprintf(stderr, "Couldn't open directory: %s\n", dirname);
    exit(-1);
  }

  while((entry=readdir(d_fh)) != NULL) {

    /* Don't descend up the tree or include the current directory */
    if(strncmp(entry->d_name, "..", 2) != 0 &&
       strncmp(entry->d_name, ".", 1) != 0) {

      /* If it's a directory print it's name and recurse into it */
      if (entry->d_type == DT_DIR) {
        for(i=0; i < 2*lvl; i++) {
          printf(" ");
        }
        printf("%s (d)\n", entry->d_name);

        /* Prepend the current directory and recurse */
        strncpy(longest_name, dirname, 4095);
        strncat(longest_name, "/", 4095);
        strncat(longest_name, entry->d_name, 4095);
        listdir(longest_name, lvl+1);
      }
      else {

        /* Print some leading space depending on the directory level */
        for(i=0; i < 2*lvl; i++) {
          printf(" ");
        }
        printf("%s\n", entry->d_name);
      }
    }
  }

  closedir(d_fh);

  return;
}
