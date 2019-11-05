/**
 * Example to show stages of compilation:
 *   + cc -E hello.c -o hello.i
 *     - shows pre-processor output
 *   + cc -S hello.i -o hello.s
 *     - shows assembler output
 *   + cc -c hello.c hello.o
 *     - shows binary object output
 *   + cc -o hello hello.c
 *     - creates linked executable output
 *
 * TODO: 
 *   + show the full chain from input file to linked executable
**/

#include <stdio.h>
#define PRINTTHIS "hello, world, yo\n"

void main()
{
	printf(PRINTTHIS);
}

