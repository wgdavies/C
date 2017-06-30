/*
 * memwatch - monitor system memory and kill off specified user that causes over-threshold condition
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <wait.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

/* Read the System Information struct and return the value of requested memory.
 * Accepts one argument:
 *    't' option returns value of 'total' member from system memory vector
 *    'f' option returns value of 'free' member from system memory vector
 * Returns the current value of requested memory vector or '0' for error condition.
 */
unsigned long checkSystemMemory ( char vector )
{    
    unsigned long memval = 0;
    struct sysinfo info;

    if ( sysinfo ( &info ) != 0 ) {
        syslog ( LOG_INFO, "sysinfo: error reading system statistics" );
    } else {
	switch ( vector ) {
	case 't':
	    memval = info.totalram;
	    break;
	case 'f':
	    memval = info.freeram;
	    break;
	default:
	    syslog ( LOG_INFO, "error: unknown vector specified %c", vector );
	}
    }

    return ( memval );
}

/* Run a tight loop to sleep for a short interval (default 100 microseconds) and sample free memory.
 * Accepts two arguments basically as a pass-through (this will be fixed later):
 *     'MINMEM': the minimum memory value threshold
 *     'memval': the current value of a selected memory vector (e.g. 'free')
 * Returns current value of 'free' memory vector.
 */
int watchMem ( unsigned long MINMEM, unsigned long memval )
{
    unsigned int usecs = 100;

    while ( MINMEM < memval ) {
	usleep ( usecs );
	memval = checkSystemMemory ( 'f' );
    }

    return ( memval );
}

/* Manage the tight loop and kill processes owned by sUID in case memory threshold is crossed.
 * Accepts two arguments:
 *     argv[1]: percentage of memory that must remain free (default 10)
 *     argv[2]: UID value of user processes (including any shell) to be terminated (default 504)
 *
 * usage example: memwatchd 10 504
 *     This sets memwatchd to contiuously monitor the system and verify that at least 10% of total 
 *     system memory is available at any given interval.  If less than 10% is available, kill all 
 *     processes belonging to UID 504 (and continue killing until all such user processes or gone
 *     and available system memory exceeds 10%). 
 *
 * NOTE that if a different user is sucking up all system memory this instance of memwatchd will 
 * be killing off the specified user!  Also note that multiple instances of memwatchd may be run 
 * simultaneously with different parameters.
 */
int main ( int argc, char *argv[] )
{
    /* command line processing */
    int numerator, sUID;
    float percent, tmpercent;

    /* process/daemon control variables */
    int ret_val = 0;
    int nochdir = 0;
    int noclose = 0;
    
    /* memory watching parameters */
    unsigned long MINMEM, memval;

    /* master process to kill all evil */
    pid_t kill_procs = -1;
    int *status = 0;

    /* looping options */
    int secs = 1;

    /* bail on non-root instantiation */
    if ( geteuid() != 0 ) {
	fprintf ( stderr, "error: must be root to run memwatchd\n" );
	exit ( 1 );
    }

    /* immediately fork() and daemonise */
    daemon ( nochdir, noclose );
    
    /* write output to syslog */ 
    openlog ( "memwatchd.log", LOG_PERROR|LOG_PID, LOG_USER );
    
    /* process command-line arguments or set defaults */
    switch ( argc ) {
    case '2':
	numerator = atoi ( argv[1] );
	sUID = 504;
	break;
    case '3':
	numerator = atoi ( argv[1] );
	sUID = atoi ( argv[2] );
	break;
    default:
	numerator = 10;
	sUID = 504;
	break;
    }

    /* set up initial values before going into loop -- this is where we can waste instructions :) */
    memval = checkSystemMemory ( 't' );
    percent = numerator / 100.00000;
    tmpercent = (float) memval * percent;
    MINMEM = (unsigned long) tmpercent;
    memval = checkSystemMemory ( 'f' );

    /* start the tight recursive loop to check memory */
    for ( ;; ) {
	/* memory threshold is lower than available memory */
	if ( MINMEM < memval ) {
	    memval = watchMem ( MINMEM, memval );
	    
	    /* over-threshold condition -- start killing stuff off */
	} else {
	    syslog ( LOG_EMERG, "kill setuid(%d) mem_free:%ld min_free:%ld", sUID, memval, MINMEM );

	    kill_procs = fork();
	    
	    if ( kill_procs == 0 ) {
		setuid ( sUID );
		signal ( SIGKILL, SIG_IGN );
		kill ( -1, SIGKILL );
		exit ( 0 );
	    } else { 
		syslog ( LOG_INFO, "error: unable to setuid ( %d )", sUID );
		sleep ( secs );
	    }

	    waitpid ( -1, status, 0 );	    
	    memval = checkSystemMemory ( 'f' );
	}
    }
    
    closelog();

    return ( ret_val );
}
