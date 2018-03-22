/***************************************************************************
 *                                                                          
 *                        C l o s e   F u n c t i o n                       
 *                        ---------------------------                       
 *      Copyright 1982 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      The close function is used to terminate access to a file / device.  
 *                                                                          
 *      Calling Sequence:                                                   
 *                                                                          
 *              ret = close(fd);                                            
 *                                                                          
 *      Where "fd" is an open file descriptor to be CLOSEd.                 
 *                                                                          
 *      Modifications:                                                      
 *      12/83: PC-DOS mods   whf                                            
 *      10/83: changed user # handling   whf                                
 *      9/83: add steve's user # mods, change _chkc   whf                   
 *      8/83: improve error handling    whf                                 
 ****************************************************************************/

#include <unistd.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

                                                /****************************/
static char eof = '\x1a';                       /* End of file for ASCII    */
                                                /****************************/
int    close(                                   /* CLEAR FUNCTION ***********/
REG     int     fd)                             /* File descriptor to close */
{                                               /****************************/
REG     FD      *fp;                            /* file pointer Temporary   */
REG     WORD    rv;                             /* return value             */
REG     WORD    xuser;                          /* User number temporary    */
                                                /*                          */
        if((fp=_chkc(fd)) == NULLFD)            /* File Open?               */
            RETERR(FAILURE,EBADF);              /*              no, quit    */
        rv = SUCCESS;                           /* assume it will work      */
        if ((fp->flags & (ISTTY|ISLPT)) == 0)   /* Character device?        */
        {                                       /* if not, handle file: *****/
          if((fp->flags & ISASCII) != 0 && (fp->flags & ISREAD) == 0)   /*  */
          {                                     /* ASCII file? not ReadOnly?*/
            if( fp->offset < fp->hiwater )      /* Have we been seeking?    */
              lseek(fd,0L,2);                   /*   Seek to EOF ifso       */
            write(fd,&eof,1);                   /* Write a ^Z character     */
          }                                     /****************************/
          if((fp->flags & DIRTY) != 0)          /* Buffer dirty?            */
            if(_blkio(fp,fp->sector,fp->buffer,1L,B_WRITE) != 1)/* write it */
              rv = FAILURE;                     /* can't                    */
          xuser = _chkuser(fp->user);           /* Check user # for change  */
          if((__OSIF(CLOSE,&(fp->fcb))&0xFF) == 0xFF)/* Close the fcb       */
            rv = FAILURE;                       /*                          */
          _uchkuser(fp->user,xuser);            /* Change user # if needed  */
        }                                       /* end of file handling *****/
        __chinit(fd);                           /* Release the space        */
        _freec(fd);                             /* Release the channel      */
        if( rv == SUCCESS )                     /* Was everything ok?       */
             return(rv);                        /* Yes, return Success      */
        else RETERR(FAILURE,EIO);               /* else let them know       */
}                                               /****************************/
