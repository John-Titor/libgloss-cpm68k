/***************************************************************************
 *                                                                          
 *                         W r i t e   F u n c t i o n                      
 *                         ---------------------------                      
 *      Copyright 1982 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      Function "write" simulates the UNIX write system call.  Any         
 *      arbitrary number of bytes are written to the file specified by file 
 *      descriptor.  No special alignment of file or buffer is required.    
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = write(fd,buffer,bytes);                               
 *      Where:                                                              
 *              fd      is an open file descriptor                          
 *              buffer  is the buffer address                               
 *              bytes   is the number of bytes to be written                
 *              ret     is the number of bytes actually written             
 *                                                                          
 *      Modifications:                                                      
 *              11-Dec-83 whf   PC-DOS mods, divide out _wrtasc & _wrtbin.  
 *              30-Nov-83 whf   Fix _wrtbin() boundary condition bug        
 *              19-Oct-83 whf   Separate out _wrtchr(), _ttyout(), _lstout()
 *                                                                          
 ****************************************************************************/

#include <unistd.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"

ssize_t write(                                  /* CLEAR FUNCTION ***********/
                                                /*                          */
        int             fd,                     /* File descriptor          */
        const void      *buff,                  /* Buffer address           */
        size_t          bytes)                  /* Number of bytes to xfer  */
                                                /*                          */
{                                               /****************************/
        REG     FD      *fp;                    /* File (ccb) pointer       */
                                                /****************************/
        if((fp=_chkc(fd)) == NULLFD)            /* Get CCB address MGL      */
                return(FAILURE);                /*      Can't: EBADF        */
                                                /* note: bytes is unsigned  */
        if(bytes == 0)                          /* Trying to write 0        */
                return(0);                      /* Yes, a wise guy!         */
                                                /*                          */
        if((fp->flags & ISREAD) != 0)           /* Check for readonly file  */
                RETERR(FAILURE,EBADF);          /* Barf if so               */
                                                /****************************/
        if( fp->flags & (ISTTY+ISLPT+ISQUE))    /* TTY, LST or QUE File?    */
                return(_wrtchr(fp,buff,bytes)); /*      Yes, handle it      */
                                                /****************************/
        if(fp->flags & ISASCII)                 /* If ascii file            */
                return(_wrtasc(fp,buff,bytes)); /* do ascii style           */
        else    return(_wrtbin(fp,buff,bytes)); /* do binary style          */
}                                               /****************************/

