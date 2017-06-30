#include "flog.h"


/**
 * Prints a formatted message (like vprintf()) to all log destinations. Flushes the file stream's buffer so that the
 * message is immediately readable.
 *
 * NOTE: Called only from this file.
 *
 *  @param level The log level (LOG_EMERG ... LOG_DEBUG)
 *  @param format The format for the message (vprintf style)
 *  @param ap Any args to format (vprintf style)
 */
static void flog_write(int level, const char *format,  va_list ap) {
    va_list cap;
    int copy = 0;

    /* Check the log level */
    if (level > loglevel)
        return;

    /* copy the argument list if needed - it can be processed only once */
    if (logfile && logfacility) {
        copy = 1;
        va_copy(cap, ap);
    }

    if (logfile) {
        /* Print the message to the given stream. */

        time_t t = time(0);
        struct tm * pt = localtime(&t);
        char buf[256];
        strftime(buf, 256, "%c", pt);
        fprintf(logfile, "%s ", buf);

        /* TODO : enhance this with logging level  */

        vfprintf(logfile, format, ap);
        fprintf(logfile, "\n");

        /* Flush the stream's buffer, so the data is readable immediately. */
        fflush(logfile);
    }

    if (logfacility) {
        if (copy) {
            vsyslog(LOG_MAKEPRI(logfacility, level), format, cap);
            va_end(cap);
        } else
            vsyslog(LOG_MAKEPRI(logfacility, level), format, ap);
    }
}

/**
 * Prints a formatted message (like printf()) to all log destinations. Flushes the file stream's buffer so that the
 * message is immediately readable.
 *
 *  @param level The log level (LOG_EMERG ... LOG_DEBUG)
 *  @param format The format for the message (printf style)
 *  @param ... Any args to format (printf style)
 */
void flog(int level, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    flog_write(level, format, ap);
    va_end(ap);
}
