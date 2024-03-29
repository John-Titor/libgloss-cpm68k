/***************************************************************************
 *                                                                          
 *                          R e a d   F u n c t i o n                       
 *                          -------------------------                       
 *      Copyright 1982 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      The read function simulates the UNIX read system call.  Any         
 *      arbitrary number of bytes may be read at any arbitrary offset in    
 *      the file.                                                           
 *      The terminal handler for read has been set up to simulate some of   
 *      the functions provided by "ioctl".                                  
 *                                                                          
 *      Edits:                                                              
 *              11-Dec-83 whf   Divide into separate files, do PC-DOS I/O   
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = read(fd,buffer,bytes);                                
 *      Where:                                                              
 *              ret     Is the number of bytes which were actually read     
 *              fd      Is an open file descriptor                          
 *              buffer  Is the buffer address                               
 *              bytes   Is the number of bytes to read                      
 *                                                                          
 ****************************************************************************/

#include <unistd.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

ssize_t read(                                   /* CLEAR FUNCTION ***********/
        int     fd,                             /* File descriptor          */
        void    *buff,                          /* -> buffer start          */
        size_t  bytes)                          /* =  byte count to read    */
{                                               /****************************/
        FD      *fp;                            /* -> ccb to read from      */
                                                /****************************/
        if((fp=_chkc(fd)) == NULLFD)            /* File open & OK??         */
                return(FAILURE);                /*      No: EBADF           */
        if((fp->flags & ATEOF) != 0)            /* End of file already?     */
                return(0);                      /* Yes, quit now            */
        if((fp->flags & ISLPT) != 0)            /* A wise guy??             */
                RETERR(FAILURE,EBADF);          /*      A wise guy!!        */
                                                /****************************/
        if((fp->flags & ISTTY) != 0)            /* TTY?                     */
                return(_ttyin(fp,buff,bytes));  /*      Yes, read 1 line    */
                                                /****************************/
        if(fp->flags & ISASCII)                 /* ASCII??                  */
                return(_rdasc(fp,buff,(LONG)bytes)); /* Yes, read ascii     */
        else    return(_rdbin(fp,buff,(LONG)bytes)); /* No, read binary     */
                                                /****************************/
}
