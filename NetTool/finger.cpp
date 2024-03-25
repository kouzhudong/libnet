#include "finger.h"

char * __progname;

time_t now;
int lflag, mflag, pplan, sflag;


/*
 * get option letter from argument vector
 */
int opterr = 1,      /* if error message should be printed */
optind = 1,      /* index into parent argv vector */
optopt;          /* character checked for validity */
const char * optarg; /* argument associated with option */


int getopt(int nargc, char * const * nargv, const char * ostr)
{
    static const char * place = EMSG; /* option letter processing */
    register char * oli;              /* option letter list index */
    char * p;

    if (!*place) { /* update scanning pointer */
        if (optind >= nargc || *(place = nargv[optind]) != '-') {
            place = EMSG;
            return (EOF);
        }
        if (place[1] && *++place == '-') { /* found "--" */
            ++optind;
            place = EMSG;
            return (EOF);
        }
    } /* option letter okay? */
    if ((optopt = (int)*place++) == (int)':' || !(oli = (char *)strchr(ostr, optopt))) {
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
    if (*++oli != ':') { /* don't need argument */
        optarg = NULL;
        if (!*place)
            ++optind;
    } else {        /* need an argument */
        if (*place) /* no white space */
            optarg = place;
        else if (nargc <= ++optind) { /* no arg */
            place = EMSG;
            if (!(p = strrchr(*nargv, '/')))
                p = *nargv;
            else
                ++p;
            if (opterr)
                (void)fprintf(stderr, "%s: option requires an argument -- %c\n", p, optopt);
            return (BADCH);
        } else /* white space */
            optarg = nargv[optind];

        place = EMSG;
        ++optind;
    }

    return (optopt); /* dump back option letter */
}


void verr(int eval, const char * fmt, va_list ap)
{
    int sverrno;

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


void err(int eval, const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verr(eval, fmt, ap);
    va_end(ap);
}


void verrx(int eval, const char * fmt, va_list ap)
{
    (void)fprintf(stderr, "%s: ", __progname);
    if (fmt != NULL)
        (void)vfprintf(stderr, fmt, ap);
    (void)fprintf(stderr, "\n");
    exit(eval);
}


void errx(int eval, const char * fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    verrx(eval, fmt, ap);
    va_end(ap);
}


void netfinger(char * name)
{
    extern int lflag;
    char c, lastc;
    struct in_addr defaddr;
    struct hostent * hp, def;
    struct servent * sp;
    struct sockaddr_in sin;
    SOCKET s;
    char * alist[1], * host;

    /* If this is a local request */
    if (!(host = rindex(name, '@')))
        return;

    *host++ = '\0';
    if (isdigit(*host) && (defaddr.s_addr = inet_addr(host)) != (unsigned long)-1) {
        def.h_name = host;
        def.h_addr_list = alist;
        def.h_addr = (char *)&defaddr;
        def.h_length = sizeof(struct in_addr);
        def.h_addrtype = AF_INET;
        def.h_aliases = 0;
        hp = &def;
    } else if (!(hp = gethostbyname(host))) {
        (void)fprintf(stderr, "finger: unknown host: %s\n", host);
        return;
    }
    if (!(sp = getservbyname("finger", "tcp"))) {
        (void)fprintf(stderr, "finger: tcp/finger: unknown service\n");
        return;
    }
    sin.sin_family = hp->h_addrtype;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = sp->s_port;
    if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("finger: socket");
        return;
    }

    /* have network connection; identify the host connected with */
    (void)printf("[%s]\n", hp->h_name);
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        fprintf(stderr, "finger: connect rc = %d", WSAGetLastError());
        (void)closesocket(s);
        return;
    }

    /* -l flag for remote fingerd  */
    if (lflag)
        send(s, "/W ", 3, 0);
    /* send the name followed by <CR><LF> */
    send(s, name, (int)strlen(name), 0);
    send(s, "\r\n", 2, 0);

    /*
     * Read from the remote system; once we're connected, we assume some
     * data.  If none arrives, we hang until the user interrupts.
     *
     * If we see a <CR> or a <CR> with the high bit set, treat it as
     * a newline; if followed by a newline character, only output one newline.
     *
     * Otherwise, all high bits are stripped; if it isn't printable and
     * it isn't a space, we can simply set the 7th bit.
     * Every ASCII character with bit 7 set is printable.
     */
    lastc = 0;
    while (recv(s, &c, 1, 0) == 1) {
        c &= 0x7f;
        if (c == 0x0d) {
            if (lastc == '\r') /* ^M^M - skip dupes */
                continue;
            c = '\n';
            lastc = '\r';
        } else {
            if (!isprint(c) && !isspace(c))
                c |= 0x40;
            if (lastc != '\r' || c != '\n')
                lastc = c;
            else {
                lastc = '\n';
                continue;
            }
        }
        putchar(c);
    }
    if (lastc != '\n')
        putchar('\n');
    putchar('\n');
    (void)closesocket(s);
}


static void userlist(int argc, char ** argv)
{
    int * used = NULL;
    char ** ap, ** nargv, ** np, ** p;
    WORD wVersionRequested;
    WSADATA wsaData;
    int iErr;

    if ((nargv = (char **)malloc(((size_t)argc + 1) * sizeof(char *))) == NULL ||
        (used = (int *)calloc(argc, sizeof(int))) == NULL)
        err(1, NULL);

    /* Pull out all network requests into nargv. */
    for (ap = p = argv, np = nargv; *p; ++p)
        if (index(*p, '@'))
            *np++ = *p;
        else
            *ap++ = *p;

    *np++ = NULL;
    *ap++ = NULL;

    /* If there are local requests */
    if (*argv) {
        fprintf(stderr, "Warning: Can't do local finger\n");
    }

    /* Start winsock */
    wVersionRequested = MAKEWORD(1, 1);
    iErr = WSAStartup(wVersionRequested, &wsaData);
    if (iErr != 0) {
        /* Tell the user that we couldn't find a usable */
        /* WinSock DLL.                                  */
        fprintf(stderr, "WSAStartup failed\n");
        free(nargv);
        free(used);
        return;
    }

    /* Handle network requests. */
    for (p = nargv; *p;)
        netfinger(*p++);

    /* Bring down winsock */
    WSACleanup();
    free(nargv);
    free(used);
    exit(0);
}


void usage()
{
    (void)fprintf(stderr, "usage: finger [-lmps] login [...]\n");
    exit(1);
}


int finger(int argc, char ** argv)
{
    int ch;

    while ((ch = getopt(argc, argv, "lmps")) != EOF)
        switch (ch) {
        case 'l':
            lflag = 1; /* long format */
            break;
        case 'm':
            mflag = 1; /* force exact match of names */
            break;
        case 'p':
            pplan = 1; /* don't show .plan/.project */
            break;
        case 's':
            sflag = 1; /* short format */
            break;
        case '?':
        default:
            (void)fprintf(stderr, "usage: finger [-lmps] login [...]\n");
            exit(1);
        }

    argc -= optind;
    argv += optind;

    (void)time(&now);
    if (!*argv) {
        usage();
    } else {
        userlist(argc, argv);
        /*
         * Assign explicit "large" format if names given and -s not
         * explicitly stated.  Force the -l AFTER we get names so any
         * remote finger attempts specified won't be mishandled.
         */
        if (!sflag)
            lflag = 1; /* if -s not explicit, force -l */
    }

    return 0;
}
