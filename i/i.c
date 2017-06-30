#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(argc, argv)
{
    int args = sizeof ( argv );
    
    if ( args == 0 ) {
	printf ( "You *SO* need to read the directions!\n" );
    } else {
	printf ( "%3d: You *SO* %s", args, (char *)argc );
    }
    
    return 0;
}
