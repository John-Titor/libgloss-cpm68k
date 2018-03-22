/***************************************************************************
 *                                                                          
 *                          L s e e k   F u n c t i o n                     
 *                          ---------------------------                     
 *      Copyright 1982 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      The lseek function changes the present position of reading          
 *      to or writing from a file.                                          
 *      The tell function returns the present position in a file.           
 *                                                                          
 *      Calling sequence:                                                   
 *                                                                          
 *              ret = lseek(fd,offset,sense);                               
 *      or      ret = tell(fd);                                             
 *                                                                          
 *      Where:                                                              
 *                                                                          
 *              fd      Is an open file descriptor                          
 *              offset  Is the (long) file offset                           
 *              sense   Is the meaning of offset                            
 *                      0 => From the beginning of the file                 
 *                      1 => From the present file position                 
 *                      2 => From the end of the file                       
 *                                                                          
 *              ret     Is the resulting absolute file offset               
 *                      -1 indicates failure                                
 *                                                                          
 ****************************************************************************/

#include <unistd.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"

LONG    lseek(                                  /* CLEAR FUNCTION ***********/
                                                /*                          */
        int     fd,                             /* Open file descriptor     */
        off_t   offs,                           /* File offset              */
        int     sense)                          /* Sense of offset          */
                                                /*                          */
                                                /****************************/
{
                                                /****************************/
REG     FD      *fp;                            /* File pointer             */
                                                /*                          */
        if((fp = _chkc(fd)) == NULLFD)          /* Convert to pointer       */
                RETERR(FAILURE,EBADF);          /*   Can't                  */
        switch(sense)                           /****************************/
        {                                       /*                          */
        case    0:                              /* From beginning of file   */
                fp->offset = offs;              /* Just set to offset       */
                break;                          /* Then quit                */
                                                /****************************/
        case    1:                              /* From present position    */
                fp->offset += offs;             /*                          */
                break;                          /*                          */
                                                /****************************/
        case    2:                              /* From end of file         */
                fp->hiwater = _filesz(fd);      /* go find the end of file  */
                fp->offset = fp->hiwater + offs;/* compute from end of file */
                break;                          /*                          */
                                                /****************************/
        default:                                /* All others NFG           */
                RETERR(FAILURE,EINVAL);         /*                          */
        }                                       /****************************/
        fp->flags &= ~ATEOF;                    /* any seek clears EOF      */
        if( fp->offset < 0 )                    /* bad seek call?           */
                fp->offset = -1;                /* set to uniform value     */
        return(fp->offset);                     /* Return resultant offset  */
}                                               /****************************/
