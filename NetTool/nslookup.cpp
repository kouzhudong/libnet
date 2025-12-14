/*
 * PROJECT:     ReactOS nslookup utility
 * LICENSE:     GPL - See COPYING in the top level directory
 * FILE:        base/applications/network/nslookup/nslookup.c
 * PURPOSE:     Perform DNS lookups
 * COPYRIGHT:   Copyright 2009 Lucas Suggs <lucas.suggs@gmail.com>
 */

#include "nslookup.h"

#include <winbase.h>
#include <iphlpapi.h>

STATE State;
HANDLE ProcessHeap;
ULONG RequestID;

#pragma warning(disable : 4996)
#pragma warning(disable : 4477)
#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#pragma warning(disable : 6386)


//////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * PROJECT:     ReactOS nslookup utility
 * LICENSE:     GPL - See COPYING in the top level directory
 * FILE:        base/applications/network/nslookup/utility.c
 * PURPOSE:     Support functions for nslookup.c
 * COPYRIGHT:   Copyright 2009 Lucas Suggs <lucas.suggs@gmail.com>
 */

#include "nslookup.h"

BOOL SendRequest(PCHAR pInBuffer, ULONG InBufferLength, PCHAR pOutBuffer, PULONG pOutBufferLength)
{
    int j;
    USHORT RequestID, ResponseID;
    BOOL bWait;
    SOCKET s;
    SOCKADDR_IN RecAddr, RecAddr2, SendAddr;
    int SendAddrLen = sizeof(SendAddr);

    RtlZeroMemory(&RecAddr, sizeof(SOCKADDR_IN));
    RtlZeroMemory(&RecAddr2, sizeof(SOCKADDR_IN));
    RtlZeroMemory(&SendAddr, sizeof(SOCKADDR_IN));

    RequestID = ntohs(((PSHORT)&pInBuffer[0])[0]);/* Pull the request ID from the buffer. */

    /* If D2 flags is enabled, then display D2 information. */
    if (State.d2)
        PrintD2(pInBuffer, InBufferLength);
    
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);/* Create the sockets for both send and receive. */
    if (s == INVALID_SOCKET)
        return FALSE;

    /* Set up the structure to tell it where we are going. */
    RecAddr.sin_family = AF_INET;
    RecAddr.sin_port = htons(State.port);
    RecAddr.sin_addr.s_addr = inet_addr(State.DefaultServerAddress);

    /* Set up the structure to tell it what port to listen on. */
    RecAddr2.sin_family = AF_INET;
    RecAddr2.sin_port = htons(State.port);
    RecAddr2.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(s, (SOCKADDR *)&RecAddr2, sizeof(RecAddr2));/* Bind the receive socket. */

    /* Send the datagram to the DNS server. */
    j = sendto(s, pInBuffer, InBufferLength, 0, (SOCKADDR *)&RecAddr, sizeof(RecAddr));
    if (j == SOCKET_ERROR) {
        switch (WSAGetLastError()) {
        case WSANOTINITIALISED:
            printf(("sendto() failed with WSANOTINITIALIZED\n"));
            break;
        case WSAENETDOWN:
            printf(("sendto() failed with WSAENETDOWN\n"));
            break;
        case WSAEACCES:
            printf(("sendto() failed with WSAEACCES\n"));
            break;
        case WSAEINVAL:
            printf(("sendto() failed with WSAEINVAL\n"));
            break;
        case WSAEINTR:
            printf(("sendto() failed with WSAEINTR\n"));
            break;
        case WSAEINPROGRESS:
            printf(("sendto() failed with WSAEINPROGRESS\n"));
            break;
        case WSAEFAULT:
            printf(("sendto() failed with WSAEFAULT\n"));
            break;
        case WSAENETRESET:
            printf(("sendto() failed with WSAENETRESET\n"));
            break;
        case WSAENOBUFS:
            printf(("sendto() failed with WSAENOBUFS\n"));
            break;
        case WSAENOTCONN:
            printf(("sendto() failed with WSAENOTCONN\n"));
            break;
        case WSAENOTSOCK:
            printf(("sendto() failed with WSAENOTSOCK\n"));
            break;
        case WSAEOPNOTSUPP:
            printf(("sendto() failed with WSAEOPNOTSUPP\n"));
            break;
        case WSAESHUTDOWN:
            printf(("sendto() failed with WSAESHUTDOWN\n"));
            break;
        case WSAEWOULDBLOCK:
            printf(("sendto() failed with WSAEWOULDBLOCK\n"));
            break;
        case WSAEMSGSIZE:
            printf(("sendto() failed with WSAEMSGSIZE\n"));
            break;
        case WSAEHOSTUNREACH:
            printf(("sendto() failed with WSAEHOSTUNREACH\n"));
            break;
        case WSAECONNABORTED:
            printf(("sendto() failed with WSAECONNABORTED\n"));
            break;
        case WSAECONNRESET:
            printf(("sendto() failed with WSAECONNRESET\n"));
            break;
        case WSAEADDRNOTAVAIL:
            printf(("sendto() failed with WSAEADDRNOTAVAIL\n"));
            break;
        case WSAEAFNOSUPPORT:
            printf(("sendto() failed with WSAEAFNOSUPPORT\n"));
            break;
        case WSAEDESTADDRREQ:
            printf(("sendto() failed with WSAEDESTADDRREQ\n"));
            break;
        case WSAENETUNREACH:
            printf(("sendto() failed with WSAENETUNREACH\n"));
            break;
        case WSAETIMEDOUT:
            printf(("sendto() failed with WSAETIMEDOUT\n"));
            break;
        default:
            printf(("sendto() failed with unknown error\n"));
        }

        closesocket(s);
        return FALSE;
    }

    bWait = TRUE;

    while (bWait) {
        /* Wait for the DNS reply. */
        j = recvfrom(s, pOutBuffer, *pOutBufferLength, 0, (SOCKADDR *)&SendAddr, &SendAddrLen);
        if (j == SOCKET_ERROR) {
            switch (WSAGetLastError()) {
            case WSANOTINITIALISED:
                printf(("recvfrom() failed with WSANOTINITIALIZED\n"));
                break;
            case WSAENETDOWN:
                printf(("recvfrom() failed with WSAENETDOWN\n"));
                break;
            case WSAEACCES:
                printf(("recvfrom() failed with WSAEACCES\n"));
                break;
            case WSAEINVAL:
                printf(("recvfrom() failed with WSAEINVAL\n"));
                break;
            case WSAEINTR:
                printf(("recvfrom() failed with WSAEINTR\n"));
                break;
            case WSAEINPROGRESS:
                printf(("recvfrom() failed with WSAEINPROGRESS\n"));
                break;
            case WSAEFAULT:
                printf(("recvfrom() failed with WSAEFAULT\n"));
                break;
            case WSAENETRESET:
                printf(("recvfrom() failed with WSAENETRESET\n"));
                break;
            case WSAENOBUFS:
                printf(("recvfrom() failed with WSAENOBUFS\n"));
                break;
            case WSAENOTCONN:
                printf(("recvfrom() failed with WSAENOTCONN\n"));
                break;
            case WSAENOTSOCK:
                printf(("recvfrom() failed with WSAENOTSOCK\n"));
                break;
            case WSAEOPNOTSUPP:
                printf(("recvfrom() failed with WSAEOPNOTSUPP\n"));
                break;
            case WSAESHUTDOWN:
                printf(("recvfrom() failed with WSAESHUTDOWN\n"));
                break;
            case WSAEWOULDBLOCK:
                printf(("recvfrom() failed with WSAEWOULDBLOCK\n"));
                break;
            case WSAEMSGSIZE:
                printf(("recvfrom() failed with WSAEMSGSIZE\n"));
                break;
            case WSAEHOSTUNREACH:
                printf(("recvfrom() failed with WSAEHOSTUNREACH\n"));
                break;
            case WSAECONNABORTED:
                printf(("recvfrom() failed with WSAECONNABORTED\n"));
                break;
            case WSAECONNRESET:
                printf(("recvfrom() failed with WSAECONNRESET\n"));
                break;
            case WSAEADDRNOTAVAIL:
                printf(("recvfrom() failed with WSAEADDRNOTAVAIL\n"));
                break;
            case WSAEAFNOSUPPORT:
                printf(("recvfrom() failed with WSAEAFNOSUPPORT\n"));
                break;
            case WSAEDESTADDRREQ:
                printf(("recvfrom() failed with WSAEDESTADDRREQ\n"));
                break;
            case WSAENETUNREACH:
                printf(("recvfrom() failed with WSAENETUNREACH\n"));
                break;
            case WSAETIMEDOUT:
                printf(("recvfrom() failed with WSAETIMEDOUT\n"));
                break;
            default:
                printf(("recvfrom() failed with unknown error\n"));
            }

            closesocket(s);
            return FALSE;
        }

        ResponseID = ntohs(((PSHORT)&pOutBuffer[0])[0]);
        if (ResponseID == RequestID)
            bWait = FALSE;
    }

    closesocket(s); /* We don't need the sockets anymore. */

    /* If debug information then display debug information. */
    if (State.debug)
        PrintDebug(pOutBuffer, j);

    *pOutBufferLength = j; /* Return the real output buffer length. */

    return TRUE;
}

void ReverseIP(PCHAR pIP, PCHAR pReturn)
{
    size_t i;
    size_t j;
    size_t k = 0;

    j = strlen(pIP) - 1;
    i = j;

    /* We have A.B.C.D
       We will turn this into D.C.B.A and stick it in pReturn */

    /* A */
    for (; i > 0; i -= 1)
        if ('.' == pIP[i])
            break;

    strncpy(&pReturn[k], &pIP[i + 1], (j - i));
    k += (j - i);

    pReturn[k] = '.';
    k += 1;

    i -= 1;
    j = i;

    /* B */
    for (; i > 0; i -= 1)
        if ('.' == pIP[i])
            break;

    strncpy(&pReturn[k], &pIP[i + 1], (j - i));
    k += (j - i);

    pReturn[k] = '.';
    k += 1;

    i -= 1;
    j = i;

    /* C */
    for (; i > 0; i -= 1)
        if ('.' == pIP[i])
            break;

    strncpy(&pReturn[k], &pIP[i + 1], (j - i));
    k += (j - i);

    pReturn[k] = '.';
    k += 1;

    i -= 1;
    j = i;

    /* D */
    for (; i > 0; i -= 1)
        ;

    strncpy(&pReturn[k], &pIP[i], (j - i) + 1);
    k += (j - i) + 1;

    pReturn[k] = '\0';
}


BOOL IsValidIP(PCHAR pInput)
{
    int i = 0, l = 0, b = 0, c = 1;

    /* Max length of an IP, e.g. 255.255.255.255, is 15 characters. */
    l = strlen(pInput);
    if (l > 15)
        return FALSE;

    /* 'b' is the count of the current segment. It gets reset after seeing a
       '.'. */
    for (; i < l; i += 1) {
        if ('.' == pInput[i]) {
            if (!b)
                return FALSE;
            if (b > 3)
                return FALSE;

            b = 0;
            c += 1;
        } else {
            b += 1;

            if ((pInput[i] < '0') || (pInput[i] > '9'))
                return FALSE;
        }
    }

    if (b > 3)
        return FALSE;

    /* 'c' is the number of segments seen. If it's less than 4, then it's not a valid IP. */
    if (c < 4)
        return FALSE;

    return TRUE;
}


int ExtractName(PCHAR pBuffer, PCHAR pOutput, USHORT Offset, UCHAR Limit)
{
    int c = 0, d = 0, i = 0, j = 0, k = 0, l = 0, m = 0;

    i = Offset;

    /* If Limit == 0, then we assume "no" limit. */
    d = Limit;
    if (0 == Limit)
        d = 255;

    while (d > 0) {
        l = pBuffer[i] & 0xFF;
        i += 1;
        if (!m)
            c += 1;

        if (0xC0 == l) {
            if (!m)
                c += 1;
            m = 1;
            d += (255 - Limit);
            i = pBuffer[i];
        } else {
            for (j = 0; j < l; j += 1) {
                pOutput[k] = pBuffer[i];

                i += 1;
                if (!m)
                    c += 1;
                k += 1;
                d -= 1;
            }

            d -= 1;

            if (!pBuffer[i] || (d < 1))
                break;

            pOutput[k] = '.';
            k += 1;
        }
    };

    if (!m) {
        if (!Limit)
            c += 1;
    }

    pOutput[k] = '\0';

    return c;
}


int ExtractIP(PCHAR pBuffer, PCHAR pOutput, USHORT Offset)
{
    int c = 0, l = 0, i = 0, v = 0;

    i = Offset;

    v = (UCHAR)pBuffer[i];
    l += 1;
    i += 1;

    sprintf(&pOutput[c], "%d.", v);
    c += strlen(&pOutput[c]);

    v = (UCHAR)pBuffer[i];
    l += 1;
    i += 1;

    sprintf(&pOutput[c], "%d.", v);
    c += strlen(&pOutput[c]);

    v = (UCHAR)pBuffer[i];
    l += 1;
    i += 1;

    sprintf(&pOutput[c], "%d.", v);
    c += strlen(&pOutput[c]);

    v = (UCHAR)pBuffer[i];
    l += 1;
    i += 1;

    sprintf(&pOutput[c], "%d", v);
    c += strlen(&pOutput[c]);

    pOutput[c] = '\0';

    return l;
}


void PrintD2(PCHAR pBuffer, DWORD BufferLength)
{
    USHORT RequestID;
    UCHAR Header1, Header2;
    USHORT NumQuestions, NumAnswers, NumAuthority, NumAdditional;
    USHORT Type, Class;
    CHAR pName[255];
    int i = 0, k = 0;

    RequestID = ntohs(((PUSHORT)&pBuffer[i])[0]);
    i += 2;

    Header1 = pBuffer[i];
    i += 1;

    Header2 = pBuffer[i];
    i += 1;

    NumQuestions = ntohs(((PSHORT)&pBuffer[i])[0]);
    i += 2;

    NumAnswers = ntohs(((PSHORT)&pBuffer[i])[0]);
    i += 2;

    NumAuthority = ntohs(((PUSHORT)&pBuffer[i])[0]);
    i += 2;

    NumAdditional = ntohs(((PUSHORT)&pBuffer[i])[0]);
    i += 2;

    printf(("------------\n"));
    printf(("SendRequest(), len %d\n"), (int)BufferLength);
    printf(("    HEADER:\n"));
    printf("        opcode = %s, id = %d, rcode = %s\n", OpcodeIDtoOpcodeName((Header1 & 0x78) >> 3), (int)RequestID, RCodeIDtoRCodeName(Header2 & 0x0F));

    printf(("        header flags:  query"));
    if (Header1 & 0x01)
        printf((", want recursion"));
    printf(("\n"));

    printf(("        questions = %d,  answers = %d,  authority records = %d,  additional = %d\n\n"), 
           (int)NumQuestions,
           (int)NumAnswers,
           (int)NumAuthority,
           (int)NumAdditional);

    if (NumQuestions) {
        printf(("    QUESTIONS:\n"));

        for (k = 0; k < NumQuestions; k += 1) {
            i += ExtractName(pBuffer, pName, i, 0);

            printf("        %s", pName);

            Type = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            Class = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            printf((", type = %s, class = %s\n"), TypeIDtoTypeName(Type), ClassIDtoClassName(Class));
        }
    }

    printf(("\n------------\n"));
}


void PrintDebug(PCHAR pBuffer, DWORD BufferLength)
{
    USHORT ResponseID;
    UCHAR Header1, Header2;
    USHORT NumQuestions, NumAnswers, NumAuthority, NumAdditional;
    USHORT Type, Class;
    ULONG TTL;
    CHAR pName[255];
    int d = 0, i = 0, k = 0;

    ResponseID = ntohs(((PUSHORT)&pBuffer[i])[0]);
    i += 2;

    Header1 = pBuffer[i];
    i += 1;

    Header2 = pBuffer[i];
    i += 1;

    NumQuestions = ntohs(((PSHORT)&pBuffer[i])[0]);
    i += 2;

    NumAnswers = ntohs(((PSHORT)&pBuffer[i])[0]);
    i += 2;

    NumAuthority = ntohs(((PUSHORT)&pBuffer[i])[0]);
    i += 2;

    NumAdditional = ntohs(((PUSHORT)&pBuffer[i])[0]);
    i += 2;

    printf(("------------\n"));
    printf(("Got answer (%d bytes):\n"), (int)BufferLength);
    printf(("    HEADER:\n"));
    printf(("        opcode = %s, id = %d, rcode = %s\n"), OpcodeIDtoOpcodeName((Header1 & 0x78) >> 3), (int)ResponseID, RCodeIDtoRCodeName(Header2 & 0x0F));

    printf(("        header flags:  response"));
    if (Header1 & 0x01)
        printf((", want recursion"));
    if (Header2 & 0x80)
        printf((", recursion avail."));
    printf(("\n"));

    printf(("        questions = %d,  answers = %d,  authority records = %d,  additional = %d\n\n"),
           (int)NumQuestions,
           (int)NumAnswers,
           (int)NumAuthority,
           (int)NumAdditional);

    if (NumQuestions) {
        printf(("    QUESTIONS:\n"));

        for (k = 0; k < NumQuestions; k += 1) {
            i += ExtractName(pBuffer, pName, i, 0);

            printf(("        %s"), pName);

            Type = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            Class = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            printf((", type = %s, class = %s\n"), TypeIDtoTypeName(Type), ClassIDtoClassName(Class));
        }
    }

    if (NumAnswers) {
        printf(("    ANSWERS:\n"));

        for (k = 0; k < NumAnswers; k += 1) {
            printf(("    ->  "));
            
            i += ExtractName(pBuffer, pName, i, 0);/* Print out the name. */

            printf(("%s\n"), pName);

            /* Print out the type, class and data length. */
            Type = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            Class = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            TTL = ntohl(((PULONG)&pBuffer[i])[0]);
            i += 4;

            d = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            printf(("        type = %s, class = %s, dlen = %d\n"), TypeIDtoTypeName(Type), ClassIDtoClassName(Class), d);

            /* Print out the answer. */
            if (TYPE_A == Type) {
                i += ExtractIP(pBuffer, pName, i);

                printf(("        internet address = %s\n"), pName);
            } else {
                i += ExtractName(pBuffer, pName, i, d);

                printf(("        name = %s\n"), pName);
            }

            printf(("        ttl = %d ()\n"), (int)TTL);
        }
    }

    if (NumAuthority) {
        printf(("    AUTHORITY RECORDS:\n"));

        for (k = 0; k < NumAuthority; k += 1) {
            /* Print out the zone name. */
            i += ExtractName(pBuffer, pName, i, 0);

            printf(("    ->  %s\n"), pName);

            /* Print out the type, class, data length and TTL. */
            Type = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            Class = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            TTL = ntohl(((PULONG)&pBuffer[i])[0]);
            i += 4;

            d = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            printf(("        type = %s, class = %s, dlen = %d\n"), TypeIDtoTypeName(Type), ClassIDtoClassName(Class), d);

            /* TODO: There might be more types? */
            if (TYPE_NS == Type) {
                /* Print out the NS. */
                i += ExtractName(pBuffer, pName, i, d);

                printf(("        nameserver = %s\n"), pName);

                printf(("        ttl = %d ()\n"), (int)TTL);
            } else if (TYPE_SOA == Type) {
                printf(("        ttl = %d ()\n"), (int)TTL);

                /* Print out the primary NS. */
                i += ExtractName(pBuffer, pName, i, 0);

                printf(("        primary name server = %s\n"), pName);

                /* Print out the responsible mailbox. */
                i += ExtractName(pBuffer, pName, i, 0);

                printf(("        responsible mail addr = %s\n"), pName);

                /* Print out the serial, refresh, retry, expire and default TTL. */
                printf(("        serial = ()\n"));
                printf(("        refresh = ()\n"));
                printf(("        retry = ()\n"));
                printf(("        expire = ()\n"));
                printf(("        default TTL = ()\n"));
                i += 20;
            }
        }
    }

    if (NumAdditional) {
        printf(("    ADDITIONAL:\n"));

        for (k = 0; k < NumAdditional; k += 1) {
            /* Print the name. */
            i += ExtractName(pBuffer, pName, i, 0);

            printf(("    ->  %s\n"), pName);

            /* Print out the type, class, data length and TTL. */
            Type = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            Class = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            TTL = ntohl(((PULONG)&pBuffer[i])[0]);
            i += 4;

            d = ntohs(((PUSHORT)&pBuffer[i])[0]);
            i += 2;

            printf(("        type = %s, class = %s, dlen = %d\n"), TypeIDtoTypeName(Type), ClassIDtoClassName(Class), d);

            /* TODO: There might be more types? */
            if (TYPE_A == Type) {
                /* Print out the NS. */
                i += ExtractIP(pBuffer, pName, i);

                printf(("        internet address = %s\n"), pName);
                printf(("        ttl = %d ()\n"), (int)TTL);
            }
        }
    }

    printf(("\n------------\n"));
}


PCHAR OpcodeIDtoOpcodeName(UCHAR Opcode)
{
    switch (Opcode & 0x0F) {
    case OPCODE_QUERY:
        return (PCHAR)OpcodeQuery;
    case OPCODE_IQUERY:
        return (PCHAR)OpcodeIQuery;
    case OPCODE_STATUS:
        return (PCHAR)OpcodeStatus;
    default:
        return (PCHAR)OpcodeReserved;
    }
}


PCHAR RCodeIDtoRCodeName(UCHAR RCode)
{
    switch (RCode & 0x0F) {
    case RCODE_NOERROR:
        return (PCHAR)RCodeNOERROR;
    case RCODE_FORMERR:
        return (PCHAR)RCodeFORMERR;
    case RCODE_FAILURE:
        return (PCHAR)RCodeFAILURE;
    case RCODE_NXDOMAIN:
        return (PCHAR)RCodeNXDOMAIN;
    case RCODE_NOTIMP:
        return (PCHAR)RCodeNOTIMP;
    case RCODE_REFUSED:
        return (PCHAR)RCodeREFUSED;
    default:
        return (PCHAR)RCodeReserved;
    }
}


PCHAR TypeIDtoTypeName(USHORT TypeID)
{
    switch (TypeID) {
    case TYPE_A:
        return (PCHAR)TypeA;
    case TYPE_NS:
        return (PCHAR)TypeNS;
    case TYPE_CNAME:
        return (PCHAR)TypeCNAME;
    case TYPE_SOA:
        return (PCHAR)TypeSOA;
    case TYPE_WKS:
        return (PCHAR)TypeSRV;
    case TYPE_PTR:
        return (PCHAR)TypePTR;
    case TYPE_MX:
        return (PCHAR)TypeMX;
    case TYPE_ANY:
        return (PCHAR)TypeAny;
    default:
        return (PCHAR) "Unknown";
    }
}


USHORT TypeNametoTypeID(PCHAR TypeName)
{
    if (!strncmp(TypeName, TypeA, strlen(TypeA)))
        return TYPE_A;
    if (!strncmp(TypeName, TypeNS, strlen(TypeNS)))
        return TYPE_NS;
    if (!strncmp(TypeName, TypeCNAME, strlen(TypeCNAME)))
        return TYPE_CNAME;
    if (!strncmp(TypeName, TypeSOA, strlen(TypeSOA)))
        return TYPE_SOA;
    if (!strncmp(TypeName, TypeSRV, strlen(TypeSRV)))
        return TYPE_WKS;
    if (!strncmp(TypeName, TypePTR, strlen(TypePTR)))
        return TYPE_PTR;
    if (!strncmp(TypeName, TypeMX, strlen(TypeMX)))
        return TYPE_MX;
    if (!strncmp(TypeName, TypeAny, strlen(TypeAny)))
        return TYPE_ANY;

    return 0;
}


PCHAR ClassIDtoClassName(USHORT ClassID)
{
    switch (ClassID) {
    case CLASS_IN:
        return (PCHAR)ClassIN;
    case CLASS_ANY:
        return (PCHAR)ClassAny;
    default:
        return (PCHAR) "Unknown";
    }
}


USHORT ClassNametoClassID(PCHAR ClassName)
{
    if (!strncmp(ClassName, ClassIN, strlen(ClassIN)))
        return CLASS_IN;
    if (!strncmp(ClassName, ClassAny, strlen(ClassAny)))
        return CLASS_ANY;

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void PrintState()
{
    printf(("Default Server:  (null)\n\n"));
    printf(("Set options:\n"));

    printf(("  "));
    if (!State.debug)
        printf(("no"));
    printf(("debug\n"));

    printf(("  "));
    if (!State.defname)
        printf(("no"));
    printf(("defname\n"));

    printf(("  "));
    if (!State.search)
        printf(("no"));
    printf(("search\n"));

    printf(("  "));
    if (!State.recurse)
        printf(("no"));
    printf(("recurse\n"));

    printf(("  "));
    if (!State.d2)
        printf(("no"));
    printf(("d2\n"));

    printf(("  "));
    if (!State.vc)
        printf(("no"));
    printf(("vc\n"));

    printf(("  "));
    if (!State.ignoretc)
        printf(("no"));
    printf(("ignoretc\n"));

    printf(("  port=%d\n"), State.port);
    printf(("  type=%s\n"), State.type);
    printf(("  class=%s\n"), State.Class);
    printf(("  timeout=%d\n"), (int)State.timeout);
    printf(("  retry=%d\n"), (int)State.retry);
    printf(("  root=%s\n"), State.root);
    printf(("  domain=%s\n"), State.domain);

    printf(("  "));
    if (!State.MSxfr)
        printf(("no"));
    printf(("MSxfr\n"));

    printf(("  IXFRversion=%d\n"), (int)State.ixfrver);

    printf(("  srchlist=%s\n\n"), State.srchlist[0]);
}

void PrintUsage()
{
    printf(("Usage:\n"
            "   nslookup [-opt ...]             # interactive mode using"
            " default server\n   nslookup [-opt ...] - server    #"
            " interactive mode using 'server'\n   nslookup [-opt ...]"
            " host        # just look up 'host' using default server\n"
            "   nslookup [-opt ...] host server # just look up 'host'"
            " using 'server'\n"));
}

BOOL PerformInternalLookup(PCHAR pAddr, PCHAR pResult)
{
    /* Needed to issue DNS packets and parse them. */
    PCHAR Buffer = NULL, RecBuffer = NULL;
    CHAR pResolve[256];
    ULONG BufferLength = 0, RecBufferLength = 512;
    int i = 0, j = 0, k = 0, d = 0;
    BOOL bOk = FALSE;

    /* Makes things easier when parsing the response packet. */
    USHORT NumQuestions;
    USHORT Type;

    if ((strlen(pAddr) + 1) > 255)
        return FALSE;

    Type = TYPE_A;
    if (IsValidIP(pAddr))
        Type = TYPE_PTR;

    /* If it's a PTR lookup then append the ARPA sig to the end. */
    if (Type == TYPE_PTR) {
        ReverseIP(pAddr, pResolve);
        strcat(pResolve, ARPA_SIG);
    } else {
        strcpy(pResolve, pAddr);
    }

    /* Base header length + length of QNAME + length of QTYPE and QCLASS */
    BufferLength = 12 + (strlen(pResolve) + 2) + 4;

    /* Allocate memory for the buffer. */
    Buffer = (PCHAR)HeapAlloc(ProcessHeap, 0, BufferLength);
    if (!Buffer) {
        printf(("ERROR: Out of memory\n"));
        goto cleanup;
    }

    /* Allocate the receiving buffer. */
    RecBuffer = (PCHAR)HeapAlloc(ProcessHeap, 0, RecBufferLength);
    if (!RecBuffer) {
        printf(("ERROR: Out of memory\n"));
        goto cleanup;
    }

    /* Insert the ID field. */
    ((PSHORT)&Buffer[i])[0] = htons(RequestID);
    i += 2;

    /* Bits 0-7 of the second 16 are all 0, except for when recursion is desired. */
    Buffer[i] = 0x00;
    if (State.recurse)
        Buffer[i] |= 0x01;
    i += 1;

    /* Bits 8-15 of the second 16 are 0 for a query. */
    Buffer[i] = 0x00;
    i += 1;

    /* Only 1 question. */
    ((PSHORT)&Buffer[i])[0] = htons(1);
    i += 2;

    /* We aren't sending a response, so 0 out the rest of the header. */
    Buffer[i] = 0x00;
    Buffer[i + 1] = 0x00;
    Buffer[i + 2] = 0x00;
    Buffer[i + 3] = 0x00;
    Buffer[i + 4] = 0x00;
    Buffer[i + 5] = 0x00;
    i += 6;

    /* Walk through the query address. Split each section delimited by '.'.
       Format of the QNAME section is length|data, etc. Last one is null */
    j = i;
    i += 1;

    for (k = 0; k < strlen(pResolve); k += 1) {
        if (pResolve[k] != '.') {
            Buffer[i] = pResolve[k];
            i += 1;
        } else {
            Buffer[j] = (i - j) - 1;
            j = i;
            i += 1;
        }
    }

    Buffer[j] = (i - j) - 1;
    Buffer[i] = 0x00;
    i += 1;

    /* QTYPE */
    ((PSHORT)&Buffer[i])[0] = htons(Type);
    i += 2;

    /* QCLASS */
    ((PSHORT)&Buffer[i])[0] = htons(CLASS_IN);

    /* Ship the request off to the DNS server. */
    bOk = SendRequest(Buffer, BufferLength, RecBuffer, &RecBufferLength);
    if (!bOk)
        goto cleanup;
    
    NumQuestions = ntohs(((PSHORT)&RecBuffer[4])[0]);/* Start parsing the received packet. */

    k = 12;

    /* We don't care about the questions section, blow through it. */
    if (NumQuestions) {
        for (i = 0; i < NumQuestions; i += 1) {
            /* Quick way to skip the domain name section. */
            k += ExtractName(RecBuffer, pResult, k, 0);
            k += 4;
        }
    }
    
    k += ExtractName(RecBuffer, pResult, k, 0);/* Skip the answer name. */

    Type = ntohs(((PUSHORT)&RecBuffer[k])[0]);
    k += 8;

    d = ntohs(((PUSHORT)&RecBuffer[k])[0]);
    k += 2;

    if (TYPE_PTR == Type) {
        k += ExtractName(RecBuffer, pResult, k, d);
    } else if (TYPE_A == Type) {
        k += ExtractIP(RecBuffer, pResult, k);
    }

cleanup:
    /* Free memory. */
    if (Buffer)
        HeapFree(ProcessHeap, 0, Buffer);
    if (RecBuffer)
        HeapFree(ProcessHeap, 0, RecBuffer);

    RequestID += 1;

    return bOk;
}


void PerformLookup(PCHAR pAddr)
{
    /* Needed to issue DNS packets and parse them. */
    PCHAR Buffer = NULL, RecBuffer = NULL;
    CHAR pResolve[256];
    CHAR pResult[256];
    ULONG BufferLength = 0, RecBufferLength = 512;
    int i = 0, j = 0, k = 0, d = 0;
    BOOL bOk = FALSE;

    /* Makes things easier when parsing the response packet. */
    UCHAR Header2;
    USHORT NumQuestions;
    USHORT NumAnswers;
    USHORT NumAuthority;
    USHORT Type;

    if ((strlen(pAddr) + 1) > 255)
        return;

    printf("Server:  %s\n", State.DefaultServer);
    printf("Address:  %s\n\n", State.DefaultServerAddress);

    if (!strcmp(TypeA, State.type) || !strcmp(TypeAAAA, State.type) || !strcmp(TypeBoth, State.type)) {
        Type = TYPE_A;
        if (IsValidIP(pAddr))
            Type = TYPE_PTR;
    } else
        Type = TypeNametoTypeID(State.type);

    /* If it's a PTR lookup then append the ARPA sig to the end. */
    if ((Type == TYPE_PTR) && IsValidIP(pAddr)) {
        ReverseIP(pAddr, pResolve);
        strcat(pResolve, ARPA_SIG);
    } else {
        strcpy(pResolve, pAddr);
    }
    
    BufferLength = 12 + (strlen(pResolve) + 2) + 4;/* Base header length + length of QNAME + length of QTYPE and QCLASS */
    
    Buffer = (PCHAR)HeapAlloc(ProcessHeap, 0, BufferLength);/* Allocate memory for the buffer. */
    if (!Buffer) {
        printf(("ERROR: Out of memory\n"));
        goto cleanup;
    }
    
    RecBuffer = (PCHAR)HeapAlloc(ProcessHeap, 0, RecBufferLength);/* Allocate memory for the return buffer. */
    if (!RecBuffer) {
        printf(("ERROR: Out of memory\n"));
        goto cleanup;
    }

    /* Insert the ID field. */
    ((PSHORT)&Buffer[i])[0] = htons(RequestID);
    i += 2;

    /* Bits 0-7 of the second 16 are all 0, except for when recursion is desired. */
    Buffer[i] = 0x00;
    if (State.recurse)
        Buffer[i] |= 0x01;
    i += 1;

    /* Bits 8-15 of the second 16 are 0 for a query. */
    Buffer[i] = 0x00;
    i += 1;

    /* Only 1 question. */
    ((PSHORT)&Buffer[i])[0] = htons(1);
    i += 2;

    /* We aren't sending a response, so 0 out the rest of the header. */
    Buffer[i] = 0x00;
    Buffer[i + 1] = 0x00;
    Buffer[i + 2] = 0x00;
    Buffer[i + 3] = 0x00;
    Buffer[i + 4] = 0x00;
    Buffer[i + 5] = 0x00;
    i += 6;

    /* Walk through the query address. Split each section delimited by '.'.
       Format of the QNAME section is length|data, etc. Last one is null */
    j = i;
    i += 1;

    for (k = 0; k < strlen(pResolve); k += 1) {
        if (pResolve[k] != '.') {
            Buffer[i] = pResolve[k];
            i += 1;
        } else {
            Buffer[j] = (i - j) - 1;
            j = i;
            i += 1;
        }
    }

    Buffer[j] = (i - j) - 1;
    Buffer[i] = 0x00;
    i += 1;

    /* QTYPE */
    ((PSHORT)&Buffer[i])[0] = htons(Type);
    i += 2;

    /* QCLASS */
    ((PSHORT)&Buffer[i])[0] = htons(ClassNametoClassID(State.Class));

    /* Ship off the request to the DNS server. */
    bOk = SendRequest(Buffer, BufferLength, RecBuffer, &RecBufferLength);
    if (!bOk)
        goto cleanup;

    /* Start parsing the received packet. */
    Header2 = RecBuffer[3];
    NumQuestions = ntohs(((PSHORT)&RecBuffer[4])[0]);
    NumAnswers = ntohs(((PSHORT)&RecBuffer[6])[0]);
    NumAuthority = ntohs(((PUSHORT)&RecBuffer[8])[0]);
    Type = 0;

    /* Check the RCODE for failure. */
    d = Header2 & 0x0F;
    if (d != RCODE_NOERROR) {
        switch (d) {
        case RCODE_NXDOMAIN:
            printf(("*** %s can't find %s: Non-existant domain\n"), State.DefaultServer, pAddr);
            break;
        case RCODE_REFUSED:
            printf(("*** %s can't find %s: Query refused\n"), State.DefaultServer, pAddr);
            break;
        default:
            printf(("*** %s can't find %s: Unknown RCODE\n"), State.DefaultServer, pAddr);
        }

        goto cleanup;
    }

    k = 12;

    if (NumQuestions) {
        /* Blow through the questions section since we don't care about it. */
        for (i = 0; i < NumQuestions; i += 1) {
            k += ExtractName(RecBuffer, pResult, k, 0);
            k += 4;
        }
    }

    if (NumAnswers) {
        /* Skip the name. */
        k += ExtractName(RecBuffer, pResult, k, 0);

        Type = ntohs(((PUSHORT)&RecBuffer[k])[0]);
        k += 8;

        d = ntohs(((PUSHORT)&RecBuffer[k])[0]);
        k += 2;

        if (TYPE_PTR == Type) {
            k += ExtractName(RecBuffer, pResult, k, d);
        } else if (TYPE_A == Type) {
            k += ExtractIP(RecBuffer, pResult, k);
        }
    }

    /* FIXME: This'll need to support more than PTR and A at some point. */
    if (!strcmp(State.type, TypePTR)) {
        if (TYPE_PTR == Type) {
            printf(("%s     name = %s\n"), pResolve, pResult);
        } else {
        }
    } else if (!strcmp(State.type, TypeA) || !strcmp(State.type, TypeAAAA) || !strcmp(State.type, TypeBoth)) {
        if ((TYPE_A == Type) /*|| (TYPE_AAAA == Type)*/) {
            if (0 == NumAuthority)
                printf(("Non-authoritative answer:\n"));

            printf(("Name:    %s\n"), pAddr);
            printf(("Address:  %s\n\n"), pResult);
        } else {
            printf("Name:    %s\n", pResult);
            printf("Address:  %s\n\n", pAddr);
        }
    }

cleanup:
    /* Free memory. */
    if (Buffer)
        HeapFree(ProcessHeap, 0, Buffer);
    if (RecBuffer)
        HeapFree(ProcessHeap, 0, RecBuffer);

    RequestID += 1;
}


BOOL ParseCommandLine(int argc, char * argv[])
{
    int i;
    BOOL NoMoreOptions = FALSE;
    BOOL Interactive = FALSE;
    CHAR AddrToResolve[256];
    CHAR Server[256];

    RtlZeroMemory(AddrToResolve, 256);
    RtlZeroMemory(Server, 256);

    if (2 == argc) {
        /* In the Windows nslookup, usage is only displayed if /? is the only
           option specified on the command line. */
        if (!strncmp("/?", argv[1], 2)) {
            PrintUsage();
            return 0;
        }
    }

    if (argc > 1) {
        for (i = 1; i < argc; i += 1) {
            if (NoMoreOptions) {
                strncpy(Server, argv[i], 255);

                /* Determine which one to resolve. This is based on whether the DNS server provided was an IP or an FQDN. */
                if (IsValidIP(Server)) {
                    strncpy(State.DefaultServerAddress, Server, 16);

                    PerformInternalLookup(State.DefaultServerAddress, State.DefaultServer);
                } else {
                    strncpy(State.DefaultServer, Server, 255);

                    PerformInternalLookup(State.DefaultServer, State.DefaultServerAddress);
                }

                if (Interactive)
                    return 1;

                PerformLookup(AddrToResolve);

                return 0;
            } else {
                if (!strncmp("-all", argv[i], 4)) {
                    PrintState();
                } else if (!strncmp("-type=", argv[i], 6)) {
                    if (!strncmp(TypeA, &argv[i][6], strlen(TypeA))) {
                        State.type = (PCHAR)TypeA;
                    } else if (!strncmp(TypeAAAA, &argv[i][6], strlen(TypeAAAA))) {
                        State.type = (PCHAR)TypeAAAA;
                    } else if (!strncmp(TypeBoth, &argv[i][6], strlen(TypeBoth))) {
                        State.type = (PCHAR)TypeBoth;
                    } else if (!strncmp(TypeAny, &argv[i][6], strlen(TypeAny))) {
                        State.type = (PCHAR)TypeAny;
                    } else if (!strncmp(TypeCNAME, &argv[i][6], strlen(TypeCNAME))) {
                        State.type = (PCHAR)TypeCNAME;
                    } else if (!strncmp(TypeMX, &argv[i][6], strlen(TypeMX))) {
                        State.type = (PCHAR)TypeMX;
                    } else if (!strncmp(TypeNS, &argv[i][6], strlen(TypeNS))) {
                        State.type = (PCHAR)TypeNS;
                    } else if (!strncmp(TypePTR, &argv[i][6], strlen(TypePTR))) {
                        State.type = (PCHAR)TypePTR;
                    } else if (!strncmp(TypeSOA, &argv[i][6], strlen(TypeSOA))) {
                        State.type = (PCHAR)TypeSOA;
                    } else if (!strncmp(TypeSRV, &argv[i][6], strlen(TypeSRV))) {
                        State.type = (PCHAR)TypeSRV;
                    } else {
                        printf(("unknown query type: %s"), &argv[i][6]);
                    }
                } else if (!strncmp("-domain=", argv[i], 8)) {
                    strcpy(State.domain, &argv[i][8]);
                } else if (!strncmp("-srchlist=", argv[i], 10)) {
                } else if (!strncmp("-root=", argv[i], 6)) {
                    strcpy(State.root, &argv[i][6]);
                } else if (!strncmp("-retry=", argv[i], 7)) {
                } else if (!strncmp("-timeout=", argv[i], 9)) {
                } else if (!strncmp("-querytype=", argv[i], 11)) {
                    if (!strncmp(TypeA, &argv[i][11], strlen(TypeA))) {
                        State.type = (PCHAR)TypeA;
                    } else if (!strncmp(TypeAAAA, &argv[i][11], strlen(TypeAAAA))) {
                        State.type = (PCHAR)TypeAAAA;
                    } else if (!strncmp(TypeBoth, &argv[i][11], strlen(TypeBoth))) {
                        State.type = (PCHAR)TypeBoth;
                    } else if (!strncmp(TypeAny, &argv[i][11], strlen(TypeAny))) {
                        State.type = (PCHAR)TypeAny;
                    } else if (!strncmp(TypeCNAME, &argv[i][11], strlen(TypeCNAME))) {
                        State.type = (PCHAR)TypeCNAME;
                    } else if (!strncmp(TypeMX, &argv[i][11], strlen(TypeMX))) {
                        State.type = (PCHAR)TypeMX;
                    } else if (!strncmp(TypeNS, &argv[i][11], strlen(TypeNS))) {
                        State.type = (PCHAR)TypeNS;
                    } else if (!strncmp(TypePTR, &argv[i][11], strlen(TypePTR))) {
                        State.type = (PCHAR)TypePTR;
                    } else if (!strncmp(TypeSOA, &argv[i][11], strlen(TypeSOA))) {
                        State.type = (PCHAR)TypeSOA;
                    } else if (!strncmp(TypeSRV, &argv[i][11], strlen(TypeSRV))) {
                        State.type = (PCHAR)TypeSRV;
                    } else {
                        printf(("unknown query type: %s"), &argv[i][6]);
                    }
                } else if (!strncmp("-class=", argv[i], 7)) {
                    if (!strncmp(ClassIN, &argv[i][7], strlen(ClassIN))) {
                        State.Class = (PCHAR)ClassIN;
                    } else if (!strncmp(ClassAny, &argv[i][7], strlen(ClassAny))) {
                        State.Class = (PCHAR)ClassAny;
                    } else {
                        printf(("unknown query class: %s"), &argv[i][7]);
                    }
                } else if (!strncmp("-ixfrver=", argv[i], 9)) {
                } else if (!strncmp("-debug", argv[i], 6)) {
                    State.debug = TRUE;
                } else if (!strncmp("-nodebug", argv[i], 8)) {
                    State.debug = FALSE;
                    State.d2 = FALSE;
                } else if (!strncmp("-d2", argv[i], 3)) {
                    State.d2 = TRUE;
                    State.debug = TRUE;
                } else if (!strncmp("-nod2", argv[i], 5)) {
                    if (State.debug)
                        printf(("d2 mode disabled; still in debug mode\n"));

                    State.d2 = FALSE;
                } else if (!strncmp("-defname", argv[i], 8)) {
                    State.defname = TRUE;
                } else if (!strncmp("-noddefname", argv[i], 10)) {
                    State.defname = FALSE;
                } else if (!strncmp("-recurse", argv[i], 8)) {
                    State.recurse = TRUE;
                } else if (!strncmp("-norecurse", argv[i], 10)) {
                    State.recurse = FALSE;
                } else if (!strncmp("-search", argv[i], 7)) {
                    State.search = TRUE;
                } else if (!strncmp("-nosearch", argv[i], 9)) {
                    State.search = FALSE;
                } else if (!strncmp("-vc", argv[i], 3)) {
                    State.vc = TRUE;
                } else if (!strncmp("-novc", argv[i], 5)) {
                    State.vc = FALSE;
                } else if (!strncmp("-msxfr", argv[i], 6)) {
                    State.MSxfr = TRUE;
                } else if (!strncmp("-nomsxfr", argv[i], 8)) {
                    State.MSxfr = FALSE;
                } else if (!strncmp("-", argv[i], 1) && (strlen(argv[i]) == 1)) {
                    /* Since we received just the plain - switch, we are going to be entering interactive mode. We also will not be parsing any more options. */
                    NoMoreOptions = TRUE;
                    Interactive = TRUE;
                } else {
                    /* Grab the address to resolve. No more options accepted past this point. */
                    strncpy(AddrToResolve, argv[i], 255);
                    NoMoreOptions = TRUE;
                }
            }
        }

        if (NoMoreOptions && !Interactive) {
            /* Get the FQDN of the DNS server. */
            PerformInternalLookup(State.DefaultServerAddress, State.DefaultServer);
            PerformLookup(AddrToResolve);
            return 0;
        }
    }

    PerformInternalLookup(State.DefaultServerAddress, State.DefaultServer); /* Get the FQDN of the DNS server. */

    return 1;
}


void InteractiveMode()
{
    printf(("Default Server:  %s\n"), State.DefaultServer);
    printf(("Address:  %s\n\n"), State.DefaultServerAddress);

    /* TODO: Implement interactive mode. */

    printf(("ERROR: Feature not implemented.\n"));
}


int nslookup(int argc, char * argv[])
{
    int i;
    ULONG Status;
    PFIXED_INFO pNetInfo = NULL;
    ULONG NetBufLen = 0;
    WSADATA wsaData;
    int ret;

    ProcessHeap = GetProcessHeap();
    RequestID = 1;

    /* Set up the initial state. */
    State.debug = FALSE;
    State.defname = TRUE;
    State.search = TRUE;
    State.recurse = TRUE;
    State.d2 = FALSE;
    State.vc = FALSE;
    State.ignoretc = FALSE;
    State.port = 53;
    State.type = (PCHAR)TypeBoth;
    State.Class = (PCHAR)ClassIN;
    State.timeout = 2;
    State.retry = 1;
    State.MSxfr = TRUE;
    State.ixfrver = 1;

    RtlZeroMemory(State.root, 256);
    RtlZeroMemory(State.domain, 256);
    for (i = 0; i < 6; i += 1)
        RtlZeroMemory(State.srchlist[i], 256);
    RtlZeroMemory(State.DefaultServer, 256);
    RtlZeroMemory(State.DefaultServerAddress, 16);

    memcpy(State.root, DEFAULT_ROOT, sizeof(DEFAULT_ROOT));

    /* We don't know how long of a buffer it will want to return. So we'll pass an empty one now and let it fail only once, instead of guessing. */
    Status = GetNetworkParams(pNetInfo, &NetBufLen);
    if (Status != ERROR_BUFFER_OVERFLOW) {
        printf(("Error in GetNetworkParams call\n"));
        return -2;
    }

    pNetInfo = (PFIXED_INFO)HeapAlloc(ProcessHeap, 0, NetBufLen);
    if (pNetInfo == NULL) {
        printf(("ERROR: Out of memory\n"));
        return -1;
    }

    /* For real this time. */
    Status = GetNetworkParams(pNetInfo, &NetBufLen);
    if (Status != NO_ERROR) {
        printf(("Error in GetNetworkParams call\n"));
        HeapFree(ProcessHeap, 0, pNetInfo);
        return -2;
    }

    strncpy(State.domain, pNetInfo->DomainName, 255);
    strncpy(State.srchlist[0], pNetInfo->DomainName, 255);
    strncpy(State.DefaultServerAddress, pNetInfo->DnsServerList.IpAddress.String, 15);

    HeapFree(ProcessHeap, 0, pNetInfo);

    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0) {
        printf(("Winsock initialization failed: %d\n"), ret);
        return ret;
    }

    switch (ParseCommandLine(argc, argv)) {
    case 0:
        /* This means that it was a /? parameter. */
        break;
    default:
        /* Anything else means we enter interactive mode. The only exception to this is when the host to resolve was provided on the command line. */
        InteractiveMode();
    }

    WSACleanup();
    return 0;
}
