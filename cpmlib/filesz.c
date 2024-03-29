/**********************************************************************
*
*               _ f i l e s z   f u n c t i o n
*               -------------------------------
*       Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*       This routine computes the size of a CP/M file, and leaves the fd
*       pointing to the end of file.  
*       Note that the file must have been previously opened.
*
*       Calling sequence:
*               size = _filesz(fd)
*       where:
*               size =  # bytes in file;
*                       0 if not disk file;
*                       -1 if I/O error
*               fd   = file descriptor returned by open.
*
***********************************************************************/

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

off_t _filesz(                                  /****************************/
        int fd)                                 /* file descriptor          */
{
        FD      *fp;                            /* file's ccb pointer       */
        BYTE    *p1, *p2;                       /* temps for buffer work    */
        LONG    xsector;                        /* where we are in file     */
        WORD    xuser;                          /* Current user #           */
                                                /****************************/
        if((fp = _chkc(fd)) == NULLFD)          /* Convert to pointer       */
                return(FAILURE);                /*  Can't: EBADF            */
        if(fp->flags&(ISTTY|ISLPT))             /* Non disk file?           */
                return(0);                      /*                          */

    if( fp->flags & DIRTY ) {                   /* File been used?          */
        fp->offset = fp->hiwater;               /* yes: this must be right  */
    } else {                                    /****************************/
        xuser = _chkuser(fp->user);             /* Check user # for change  */
        __OSIF(FILSIZ,&(fp->fcb));              /* Do BDOS call             */
        _uchkuser(fp->user,xuser);              /* Change back if needed    */
#if HILO                                        /*--------------------------*/
        fp->fcb.record &= 0xFFFFFFL;            /* mask off cr byte         */
        xsector = fp->fcb.record - 1;           /* last sector in file      */
#else                                           /*--------------------------*/
        xsector = (fp->fcb.record >>8) - 1;     /* last sector in file      */
#endif                                          /*--------------------------*/
        if( fp->flags & ISASCII )               /****************************/
        {                                       /* Ascii file: posit to EOF */
          if(xsector >= 0)                      /* if any size at all       */
          {                                     /*      *********************/
            if( xsector != fp->sector )         /* need to change sector?   */
            {                                   /* get correct sector********/
              if((fp->flags & DIRTY) != 0)      /* Something to write 1st?  */
              {                                 /*                          */
                if(_blkio(fp,fp->sector,fp->buffer, /* Write out            */
                  1L,B_WRITE) != 1)             /*      the buffer          */
                    RETERR(FAILURE,EIO);        /*                          */
                fp->flags &= ~DIRTY;            /* turn off this bit        */
              }                                 /*                          */
              if(_blkio(fp,xsector,fp->buffer,  /* scan this last sector    */
                  1L,B_READ) != 1)              /*    for EOFCHAR           */
                RETERR(FAILURE,EIO);            /*                          */
            }                                   /* got correct sector********/
            fp->sector = xsector;               /*                          */
            fp->offset = xsector << 7;          /*                          */
            p1 = &(fp->buffer[0]);              /* start of buf             */
            p2 = &(fp->buffer[SECSIZ]);         /* end of buf               */
            while( p1<p2  &&  *p1 != EOFCHAR)   /* look for EOFCHAR         */
                p1++;                           /*                          */
            fp->offset += p1-&(fp->buffer[0]);  /* update offset to EOF     */
          }                                     /*      *********************/
          else fp->offset=0;                    /* o.w. beginning of file   */
                                                /****************************/
        } else {                                /* Binary file:compute size */
          fp->offset = (xsector+1) << 7;        /* set to end of last record*/
        }                                       /****************************/
    }                                           /****************************/
    fp->hiwater = fp->offset;                   /* whatever happened, set it*/
    fp->flags |= ATEOF;                         /* make sure they know      */
    return(fp->offset);                         /* this is it               */
}                                               /****************************/
