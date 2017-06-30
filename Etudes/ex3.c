#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int i;

    for ( i = 1 ; i <= 150 ; i++ ) {
        printf ("%3d ", i);

        if ( i % 3 == 0 ) {
	    printf ("Fizz");
        }
	if ( i % 5 == 0 )  {
	    printf ("Buzz");
        }
	
        printf("\n");
    }

    return ( 0 );
}
