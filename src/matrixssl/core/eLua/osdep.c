/*
 * osdep.c
 *
 *  Created on: Apr 25, 2012
 *      Author: Lukas Kacer
 *      based on POSIX/osdep.c file
 */
/*
        Universal system headers and then PScore/coreApi.h
        OS-specific header includes should be added to PScore/osdep.h
*/
#include <stdlib.h>     /* abort() */
#include <fcntl.h>      /* open(), O_RDONLY, ect... */
#include <unistd.h>     /* close() */
#include <errno.h>      /* errno */
//#include <sys/time.h>   /* gettimeofday */
#include "trace.h"


#include "../coreApi.h"

/******************************************************************************/
/*
        TIME FUNCTIONS
*/
/******************************************************************************/
/*
        Module open and close
*/
int osdepTimeOpen(void)
{
/*
        Just a check if we have system timer
*/
  if( platform_timer_sys_available() == 0 ) {
      return PS_FAILURE;
  } else {
      return PS_SUCCESS;
  }
}

void osdepTimeClose()
{
}

/*
        PScore Public API implementations

        This function always returns seconds/ticks AND OPTIONALLY populates
        whatever psTime_t happens to be on the given platform.
*/
int32 psGetTime(psTime_t *t)
{
  t = platform_timer_read_sys();
  return t;
}

int32 psDiffMsecs(psTime_t then, psTime_t now)
{
  // systimer is in usec, so if it really should be in msec then divide with 1000
  return (now - then) / 1000;
}

int32 psCompareTime(psTime_t a, psTime_t b)
{
/*
        Time comparison.  1 if 'a' is less than or equal.  0 if 'a' is greater
*/
  if( a < b ) {
      return 1;
  } else {
      return 0;
  }
}


/******************************************************************************/

/******************************************************************************/
/*
        ENTROPY FUNCTIONS
*/
/******************************************************************************/
#define MAX_RAND_READS          1024

//static int32 urandfd = -1;
//static int32 randfd = -1;
/*
        Module open and close
*/
int osdepEntropyOpen(void)
{
///*
//        Open /dev/random access non-blocking.
//*/
//        if ((urandfd = open("/dev/urandom", O_RDONLY)) < 0) {
//                psErrorInt("open of urandom failed %d\n", urandfd);
//                return PS_PLATFORM_FAIL;
//        }
///*
//        For platforms that don't have /dev/random, just assign it to urandom
//*/
//        if ((randfd = open("/dev/random", O_RDONLY | O_NONBLOCK)) < 0) {
//                randfd=urandfd;
//        }
//        return PS_SUCCESS;
}

void osdepEntropyClose()
{
//        if (randfd != urandfd) {
//                close(randfd);
//        }
//        close(urandfd);
}

/*
        PScore Public API implementations
*/
int32 psGetEntropy(unsigned char *bytes, uint32 size)
{
/*
        Read from /dev/random non-blocking first, then from urandom if it would
        block.  Also, handle file closure case and re-open.
*/
//        int32                   rc, sanity, retry, readBytes;
//        unsigned char   *where = bytes;
//
//        sanity = retry = rc = readBytes = 0;
//
//        while (size) {
//                if ((rc = read(randfd, where, size)) < 0 || sanity > MAX_RAND_READS) {
//                        if (errno == EINTR) {
//                                if (sanity > MAX_RAND_READS) {
//                                        psTraceCore("psGetEntropy failed randfd sanity\n");
//                                        return PS_PLATFORM_FAIL;
//                                }
//                                sanity++;
//                                continue;
//                        } else if (errno == EAGAIN) {
//                                break;
//                        } else if (errno == EBADF && retry == 0) {
//                                close(randfd);
//                                if ((randfd = open("/dev/random", O_RDONLY | O_NONBLOCK)) < 0) {
//                                        break;
//                                }
//                                retry++;
//                                continue;
//                        } else {
//                                break;
//                        }
//                }
//                readBytes += rc;
//                where += rc;
//                size -= rc;
//        }
//
//        sanity = retry = 0;
//        while (size) {
//                if ((rc = read(urandfd, where, size)) < 0 || sanity > MAX_RAND_READS) {
//                        if (errno == EINTR) {
//                                if (sanity > MAX_RAND_READS) {
//                                        psTraceCore("psGetEntropy failed urandfd sanity\n");
//                                        return PS_PLATFORM_FAIL;
//                                }
//                                sanity++;
//                                continue;
//                        } else if (errno == EBADF && retry == 0) {
//                                close(urandfd);
//                                if ((urandfd =
//                                                open("/dev/urandom", O_RDONLY | O_NONBLOCK)) < 0) {
//                                        psTraceCore("psGetEntropy failed urandom open\n");
//                                        return PS_PLATFORM_FAIL;
//                                }
//                                retry++;
//                                continue;
//                        } else {
//                                psTraceIntCore("psGetEntropy fail errno %d\n", errno);
//                                return PS_PLATFORM_FAIL;
//                        }
//                }
//                readBytes += rc;
//                where += rc;
//                size -= rc;
//        }
//        return readBytes;
}
/******************************************************************************/


/******************************************************************************/
/*
        RAW TRACE FUNCTIONS
*/
/******************************************************************************/

int osdepTraceOpen(void)
{
  /* TODO: make check, if dbgu and/or uart is initialized. also
   * dm_init() have to be already called.
   */
  return PS_SUCCESS;
}

void osdepTraceClose(void)
{
}

void _psTrace(char *msg)
{
  TRACE_INFO("%s", msg);
}

/* message should contain one %s, unless value is NULL */
void _psTraceStr(char *message, char *value)
{
  if (value) {
    fprintf( stderr, message, value);
  } else {
    TRACE_INFO("%s", message);
  }
}

/* message should contain one %d */
void _psTraceInt(char *message, int32 value)
{
  fprintf( stderr, message, value);
}

/* message should contain one %p */
void _psTracePtr(char *message, void *value)
{
  fprintf( stderr, message, value);
}

