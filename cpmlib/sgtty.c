/*************************************************************************
*
*                       stty / gtty   Functions
*                       -----------------------
*       Copyright 1983 by Digital Reserach Inc.  All rights reserved.
*
*       "stty(fd,argp)" sets the bits for CON: file opened on 'fd'
*       "gtty(fd,argp)" gets the bits for CON: file opened on 'fd'
*
*       Where:
*               fd  = file descriptor returned by 'open'
*               argp pts to struct 'sgttyb' (see "sgtty.h")
*
**************************************************************************/
#include "prototypes.h"
#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "sgtty.h"

WORD    stty(                                   /* CLEAR FUNCTION ***********/
    WORD fd,                                    /* opened file descriptor */
    struct sgttyb *argp)                        /* ptr to control info */
{
        FD *fp;

        if( (fp=_chkc(fd)) == NULLFD )
                RETERR(FAILURE,EBADF);
        if( (fp->flags & ISTTY) == 0 )
                RETERR(FAILURE,ENOTTY);
        blkmove(argp,&(fp->fcb),sizeof(*argp)); /* no checks: do whatever */
        return 0;
}


WORD    gtty(                                   /* CLEAR FUNCTION ***********/
    WORD fd,                                    /* opened file descriptor */
    struct sgttyb *argp)                        /* ptr to control info */
{
        FD *fp;

        if( (fp=_chkc(fd)) == NULLFD )
                RETERR(FAILURE,EBADF);
        if( (fp->flags & ISTTY) == 0 )
                RETERR(FAILURE,ENOTTY);
        blkmove(&(fp->fcb),argp,sizeof(*argp)); /* no checks: do whatever */
        return 0;
}
