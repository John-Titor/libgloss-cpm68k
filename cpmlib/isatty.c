/*************************************************************************
*
*                       Channel Info Functions
*                       ----------------------
*       Copyright 1982,83 by Digital Reserach Inc.  All rights reserved.
*
*       "isatty(fd)" == TRUE iff the file on channel fd is a 'CON:' device.
*
*       Where:
*               fd  = file descriptor returned by 'open'
*
**************************************************************************/

#include <unistd.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

int isatty(                                     /* CLEAR FUNCTION ***********/
        int fd)                                 /* file des returned by open*/
{                                               /****************************/
        FD *fp;                                 /* ptr to ccb               */
                                                /*                          */
        if((fp=_chkc(fd)) == NULLFD)            /* make sure its open  MGL  */
                RETERR(0, EBADF);               /*   isno TTY ifnot open    */
        if( (fp->flags & ISTTY) == 0 )          /* test this flag           */
                RETERR(0, ENOTTY);              /*   not a TTY              */
        return 1;                               /* must be a TTY            */
}                                               /****************************/
