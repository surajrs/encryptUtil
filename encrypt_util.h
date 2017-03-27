#ifndef _ENCRYPT_UTIL
#define _ENCRYPT_UTIL


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define UCHAR unsigned char
#define INT8 signed char
#define UINT32 unsigned int
#define INT  int

//#define DEBUG 

/* Uncommenting the flag would let the utility process the buffer key length at a time.
   The default implemenation reads a large buffer and process each buffer together after
   automatically creating a key buffer whose size is equal to the buffer size.
*/
//#define PROCESS_SINGLE_KEY

#endif //_ENCRYPT_UTIL
