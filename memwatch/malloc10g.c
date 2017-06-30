#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	long int MEM = 10000000000;
	size_t FILL  =  9999999999;
	int c = 0;
	int *memloc;

	printf ( "allocating %ld MB of memory... ", MEM/1024/1024 );
	memloc = malloc ( MEM );
	memset ( memloc, c, FILL ); 
	printf ( "done\n" );

	sleep ( 60 );

	printf ( "freeing memory at location %p... ", memloc );
	free ( memloc );
	printf ( "done\n" );

	return ( 0 );
}

