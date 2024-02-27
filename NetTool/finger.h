/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Tony Nardo of the Johns Hopkins University/Applied Physics Lab.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * 8/2/97 - Ted Felix <tfelix@fred.net>
 *          Ported to Win32 from 4.4BSD-LITE2 at wcarchive.
 *          NT Workstation already has finger, and it runs fine under
 *          Win95.  Thought I'd do this anyways since not everyone has
 *          access to NT.
 *          Had to remove local handling.	 Otherwise, same as whois.
 */

 /*
  * Finger prints out information about users.  It is not portable since
  * certain fields (e.g. the full user name, office, and phone numbers) are
  * extracted from the gecos field of the passwd file which other UNIXes
  * may not have or may use for other things.
  *
  * There are currently two output formats; the short format is one line
  * per user and displays login name, tty, login time, real name, idle time,
  * and office location/phone number.  The long format gives the same
  * information (in a more legible format) as well as home directory, shell,
  * mail info, and .plan/.project files.
  */

#pragma once

#include "pch.h"

#define WIN32_NO_STATUS
#include <windef.h>
#define _INC_WINDOWS

typedef float f4byte_t;
typedef double f8byte_t;
typedef long uid_t;  // SunOS 5.5

#define __P(x) x

/* utmp.h */
#define UT_LINESIZE 8
#define UT_HOSTSIZE 16

/* stat.h */
#define  S_ISREG(mode)   (((mode)&0xF000) == 0x8000)
#define  S_ISDIR(mode)   (((mode)&0xF000) == 0x4000)

#undef MIN //take care of windows default
#undef MAX //take care of windows default
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define bcopy(s1, s2, n)  memmove(s2, s1, n)
#define bcmp(s1, s2, n)  (memcmp(s1, s2, n) != 0)
#define bzero(s, n)  memset(s, 0, n)

#define index(s, c)  strchr(s, c)
#define rindex(s, c)  strrchr(s, c)


int finger(int argc, char ** argv);
