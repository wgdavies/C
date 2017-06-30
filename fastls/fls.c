/** This program displays a count of all files under the current directory.
 *
 */

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>

int ret_val = EXIT_SUCCESS;

int main( int argc, char *argv[] )
{
    DIR *d;
    struct dirent *dir;
    char* dirpath;
    unsigned long numfiles = 0;

    if ( argc > 1 ) {
	dirpath = argv[1];
    } else { 
	dirpath = ".";
    }

    d = opendir(dirpath);
    
    if ( d ) {
	while ( (dir = readdir(d)) != NULL ) {
	    ++numfiles;
	}
	
	closedir(d);
    } else {
	printf("fls error: unable to open directory %s\n", dirpath);
	ret_val = EXIT_FAILURE;
    }
    
    printf("%s file count: %lu\n", dirpath, numfiles - 2);

    exit(ret_val);
}
