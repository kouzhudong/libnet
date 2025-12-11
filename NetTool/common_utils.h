#pragma once

#include "pch.h"

// RAII wrapper for Winsock initialization
class WinsockInitializer {
private:
    BOOL initialized;
    int errorCode;

public:
    WinsockInitializer(WORD versionRequested = MAKEWORD(2, 2)) : initialized(FALSE), errorCode(0) {
        WSADATA wsaData;
        errorCode = WSAStartup(versionRequested, &wsaData);
        initialized = (errorCode == 0);
    }

    ~WinsockInitializer() {
        if (initialized) {
            WSACleanup();
        }
    }

    BOOL IsInitialized() const {
        return initialized;
    }

    int GetErrorCode() const {
        return errorCode;
    }

    // Delete copy constructor and assignment operator
    WinsockInitializer(const WinsockInitializer&) = delete;
    WinsockInitializer& operator=(const WinsockInitializer&) = delete;
};

// Common getopt implementation (from BSD)
// Global variables for getopt
extern int opterr, optind, optopt;
extern const char* optarg;

#define BADCH   (int)'?'
#define EMSG    ""

// getopt function declaration
int getopt(int nargc, char* const* nargv, const char* ostr);

// Error handling functions (from BSD)
void verr(int eval, const char* fmt, va_list ap);
void err(int eval, const char* fmt, ...);
void verrx(int eval, const char* fmt, va_list ap);
void errx(int eval, const char* fmt, ...);
