#ifndef __OBJ2H_MACRO_H__
#define __OBJ2H_MACRO_H__

//#define _WIN32

#define KDEBUG
#ifdef KDEBUG
#define dprintf(x) printf("dp %s %d:",__FILE__,__LINE__);printf x
#else
#define dprintf(x)
#endif

#if defined(_WIN32)
#include <sys/timeb.h>
#else
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#endif

#endif


