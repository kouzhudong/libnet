/*
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 8/1/97 - Ted Felix <tfelix@fred.net>
 *          Ported to Win32 from 4.4-BSDLITE2 from wcarchive.
 *          Added WSAStartup()/WSACleanup() and switched from the
 *          more convenient fdopen()/fprintf() to send()/recv().
 */

#include "whois.h"

static char NICHOST[] = "whois.internic.net";


static void getwhoisserver(int argc, char ** argv, char ** outHost, int * outOptset)
{
    int i = 1;

    while (i < argc) {
        if (!strcmp(argv[i], "-h")) {
            if (i + 2 < argc) {
                *outHost = argv[i + 1];
                *outOptset = i + 1;
            } else {
                *outOptset = argc;
            }
            return;
        }
        i++;
    }
    *outHost = NICHOST;
    *outOptset = 1;
}


int whois(int argc, char ** argv)
{
    char ch;
    struct sockaddr_in sin;
    struct hostent * hp;
    struct servent * sp;
    SOCKET s = INVALID_SOCKET;
    int status = 0;
    char * host = NULL;
    int optset = 0;

    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    getwhoisserver(argc, argv, &host, &optset);
    argc -= optset;
    argv += optset;

    if (!host || !argc) {
        (void)fprintf(stderr, "usage: whois [-h hostname] name ...\n");
        return 1;
    }

    wVersionRequested = MAKEWORD(1, 1);
    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        perror("whois: WSAStartup failed");
        return 1;
    }

    do {
        hp = gethostbyname(host);
        if (hp == NULL) {
            (void)fprintf(stderr, "whois: %s: ", host);
            status = 1;
            break;
        }
        host = hp->h_name;

        s = socket(hp->h_addrtype, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET) {
            perror("whois: socket");
            status = 1;
            break;
        }

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = hp->h_addrtype;
        if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("whois: bind");
            status = 1;
            break;
        }

        memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
        sp = getservbyname("nicname", "tcp");
        if (sp == NULL) {
            (void)fprintf(stderr, "whois: nicname/tcp: unknown service\n");
            status = 1;
            break;
        }

        sin.sin_port = sp->s_port;

        (void)printf("[%s]\n", hp->h_name);

        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            fprintf(stderr, "whois: connect error = %d\n", WSAGetLastError());
            status = 1;
            break;
        }

        while (argc-- > 1) {
            send(s, *argv, (int)strlen(*argv), 0);
            send(s, " ", 1, 0);
            argv++;
        }
        send(s, *argv, (int)strlen(*argv), 0);
        send(s, "\r\n", 2, 0);

        while (recv(s, &ch, 1, 0) == 1)
            putchar(ch);
    } while (0);

    if (s != INVALID_SOCKET)
        closesocket(s);
    WSACleanup();
    return status;
}
