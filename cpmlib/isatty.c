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
#include "prototypes.h"

int isatty(                                     /* CLEAR FUNCTION ***********/
        int fd)                                 /* file des returned by open*/
{                                               /****************************/
        REG FD *fp;                             /* ptr to ccb               */
                                                /*                          */
        if((fp=_chkc(fd)) == NULLFD)            /* make sure its open  MGL  */
                return(0);                      /*   isno TTY ifnot open    */ // XXX should EBADF
        return( (fp->flags & ISTTY) != 0 );     /* test this flag           */
}                                               /****************************/
