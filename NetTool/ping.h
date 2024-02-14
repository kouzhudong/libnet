#pragma once
 
#include "pch.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFAULT_DATA_SIZE      32       // default data size
#define DEFAULT_SEND_COUNT     4        // number of ICMP requests to send
#define DEFAULT_RECV_TIMEOUT   6000     // six second
#define DEFAULT_TTL            128
#define MAX_RECV_BUF_LEN       0xFFFF   // Max incoming packet size.

//////////////////////////////////////////////////////////////////////////////////////////////////
 
int ping(int argc, char ** argv);