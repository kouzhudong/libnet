#include "common_utils.h"
#include <errno.h>

// Global variables for getopt
int opterr = 1;       // if error message should be printed
int optind = 1;       // index into parent argv vector
int optopt;           // character checked for validity
const char* optarg;   // argument associated with option

#pragma warning(disable : 28182)

int getopt(int nargc, char* const* nargv, const char* ostr)
{
    static const char* place = EMSG; // option letter processing
    register char* oli;              // option letter list index
    char* p;

    if (!*place) { // update scanning pointer
        if (optind >= nargc || *(place = nargv[optind]) != '-') {
            place = EMSG;
            return (EOF);
        }
        if (place[1] && *++place == '-') { // found "--"
            ++optind;
            place = EMSG;
            return (EOF);
        }
    } // option letter okay?
    if ((optopt = (int)*place++) == (int)':' || !(oli = (char*)strchr(ostr, optopt))) {
        /*
         * if the user didn't specify '-' as an option, assume it means EOF.
         */
        if (optopt == (int)'-')
            return (EOF);
        if (!*place)
            ++optind;
        if (opterr) {
            if (!(p = strrchr(*nargv, '/')))
                p = *nargv;
            else
                ++p;
            (void)fprintf(stderr, "%s: illegal option -- %c\n", p, optopt);
        }
        return (BADCH);
    }
    if (*++oli != ':') { // don't need argument
        optarg = NULL;
        if (!*place)
            ++optind;
    }
    else {        // need an argument
        if (*place) // no white space
            optarg = place;
        else if (nargc <= ++optind) { // no arg
            place = EMSG;
            if (!(p = strrchr(*nargv, '/')))
                p = *nargv;
            else
                ++p;
            if (opterr)
                (void)fprintf(stderr, "%s: option requires an argument -- %c\n", p, optopt);
            return (BADCH);
        }
        else // white space
            optarg = nargv[optind];

        place = EMSG;
        ++optind;
    }

    return (optopt); // dump back option letter
}

void verr(int eval, const char* fmt, va_list ap)
{
    int sverrno;
    extern char* __progname;

    sverrno = errno;
    (void)fprintf(stderr, "%s: ", __progname);
    if (fmt != NULL) {
        (void)vfprintf(stderr, fmt, ap);
        (void)fprintf(stderr, ": ");
    }

#pragma warning(push)
#pragma warning(disable : 4996)
    (void)fprintf(stderr, "%s\n", strerror(sverrno));
#pragma warning(pop)

    exit(eval);
}

void err(int eval, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verr(eval, fmt, ap);
    va_end(ap);
}

void verrx(int eval, const char* fmt, va_list ap)
{
    extern char* __progname;
    (void)fprintf(stderr, "%s: ", __progname);
    if (fmt != NULL)
        (void)vfprintf(stderr, fmt, ap);
    (void)fprintf(stderr, "\n");
    exit(eval);
}

void errx(int eval, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verrx(eval, fmt, ap);
    va_end(ap);
}
