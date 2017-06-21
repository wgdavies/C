// C program to print all permutations with duplicates allowed
//
// this is about the most inefficient implementation possible
// partly to just get a POC running
// and partly to see how much better we can make it!

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define dictionary "/usr/share/dict/words"

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
   3. Ending index of the string. */
void permute(char *a, int l, int r)
{
   int i;
   if (l == r)
       // printf("%s\n", a);
       fileSearch(dictionary, a);
   else
   {
       for (i = l; i <= r; i++)
       {
          swap((a+l), (a+i));
          permute(a, l+1, r);
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

    char *str = argv[1];
    // generate list of permutations
    // e.g char str[] = "asprin";
    int n = strlen(str);
    permute(str, 0, n-1);

    // check permutations against dictionary
    //

    // ideally, slurp dictionary into memory
    //
    
    // count number of matches and exit on absolute value of (num - 1)
    //
    exit(0);
}
