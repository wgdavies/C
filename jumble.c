// C program to print all permutations with duplicates allowed
//
// this is about the most inefficient implementation possible
// partly to just get a POC running
// and partly to see how much better we can make it!
//
// features and command-line options:
//  - override max file/dictionary size (default 10MiB)
//  - set number of matches to return (default 1, 0 returns all matches)
//  - return partial string matches (e.g 'sprain' also matches 'unsprained')
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define DICTIONARY "/usr/share/dict/words"
#define MAX_DICT_SIZE 10485760
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

int fileSearch(char *fname, char *str) {
    FILE *fp;
    int line_num = 1;
    int find_result = 0;
    char temp[512];
    
    if((fp = fopen(fname, "r")) == NULL) {
	return(-1);
    }
    
    while(fgets(temp, 512, fp) != NULL) {
	if((strstr(temp, str)) != NULL) {
	    printf("A match found on line: %d\n", line_num);
	    printf("\n%s\n", temp);
	    find_result++;
	}
	line_num++;
    }
    
    if(find_result == 0) {
	printf("\nSorry, couldn't find a match.\n");
    }
    
    //Close the file if still open.
    if(fp) {
	fclose(fp);
    }
    return(0);
}

/* Function to swap values at two pointers */
void swap(char *x, char *y)
{
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}
 
/* Function to print permutations of string
   This function takes three parameters:
   1. String
   2. Starting index of the string
   3. Ending index of the string.
   4. pointer to memory-mapped DICTIONARY file
*/
void permute(char *a, int l, int r, const void *dict, size_t strsz)
{
   int i;
   if (l == r)
       // printf("%s\n", a);
       // fileSearch(DICTIONARY, a);
       memmem(dict, MAX_DICT_SIZE, a, strsz);
   else
   {
       for (i = l; i <= r; i++)
       {
          swap((a+l), (a+i));
          permute(a, l+1, r, DICTIONARY, strsz);
          swap((a+l), (a+i)); //backtrack
       }
   }
}
 
/* Driver program to test above functions */
int main(int argc, char *argv[])
{
    if(argc != 2) {
	printf("usage: %s %s\n", argv[0], "<word>");
	exit(1);
    }
    // replace this crap with getopt(), popt(), &c
    char *str = argv[1];
    int n = strlen(str);
    int fd; // file descriptor, as opposed to file pointer, ahem
    size_t sz = (size_t)n;
    const char *dict;
    
    // slurp local DICTIONARY into memory
    //
    // FILE *fp = fopen(DICTIONARY, "r");
    fd = open(DICTIONARY, O_RDONLY);

    // if (fp != NULL) {
    if (fd > 0 ) {
	// dict = mmap(NULL, MAX_DICT_SIZE, PROT_READ, MAP_FILE, fp, 0);
	dict = mmap(NULL, MAX_DICT_SIZE, PROT_READ, MAP_FILE, fd, 0);
	if (dict == MAP_FAILED) { handle_error("mmap"); }
    }

    // fclose(fp);
    close(fd);
    
    // check permutations against DICTIONARY
    //
    permute(str, 0, n-1, dict, sz);

    // release memory
    //
    munmap(&dict, MAX_DICT_SIZE);
    
    // count number of matches and exit on absolute value of (num - 1)
    //
    exit(0);
}
