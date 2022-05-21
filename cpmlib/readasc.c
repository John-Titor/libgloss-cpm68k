/***************************************************************************
*                                                                           
*                         R d a s c   F u n c t i o n                       
*                         ---------------------------                       
*                                                                           
*       The "_rdasc" function performs a read operation for ascii  files.   
*       All Return chars (0x13) are eliminated (and not counted) from input.
*                                                                           
*       Calling Sequence:                                                   
*               ret = _rdasc(fp,buffer,bytes);                              
*                                                                           
*       Where:                                                              
*               fp      -> to the open CCB                                  
*               buffer  -> the user's buffer                                
*               bytes   =  the number of bytes to be read                   
*               ret     =  the number of bytes actually read                
*
*       Edits:
*       20-jan-84 whf   Handle PCDOS ^Z bug
****************************************************************************/

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

ssize_t _rdasc(                                 /****************************/
                                                /*                          */
        FD      *fp,                            /* -> ccb for the file      */
        BYTE    *buff,                          /* -> buffer to receive data*/
        size_t  bytes)                          /* =  # bytes to xfer       */
{                                               /****************************/


        BYTE    *p1;                            /* Temp byte pointer        */
        BYTE    c;                              /* Temp char                */
        LONG    xsector;                        /* Temp sector number       */
        LONG    xbytes;                         /* byte count temp          */
                                                /****************************/
    xbytes = bytes;                             /* Remember org. request    */
    while(bytes > 0)                            /* Until read is satisfied  */
    {                                           /****************************/
        xsector = fp->offset >> 7;              /* Calc starting sector     */
        if(xsector != fp->sector)               /* Match sector in buffer?? */
        {                                       /* No, must read first      */
                                                /****************************/
          if((fp->flags & DIRTY) != 0)          /* Buffer dirty??           */
          {                                     /* Yes, must write it       */
            if(_blkio(fp,fp->sector,fp->buffer, /* Try to write buffer      */
                1L,B_WRITE) != 1)               /*                          */
                RETERR(FAILURE,EIO);            /* Unable to write, quit    */
            fp->flags &= ~DIRTY;                /* Wipe dirty bit           */
          }                                     /****************************/
          if(_blkio(fp,xsector,fp->buffer,1L,   /* Read proper sector       */
                B_READ) != 1)                   /*                          */
          {                                     /* Assume no sparse ascii fs*/
                fp->flags |= ATEOF;             /* Set EOF flag             */
                return(xbytes-bytes);           /* Do partial read          */
          }
          fp->sector = xsector;                 /* Mark buffer correctly    */
        }                                       /****************************/
        p1 = &(fp->buffer[(int)fp->offset&(SECSIZ-1)]); /* Load byte pointer*/
                                                /*                          */
        while(p1 < &(fp->buffer[SECSIZ]) &&     /* Ie, more data in buffer  */
              bytes > 0)                        /* And request not satisfied*/
        {                                       /*                          */
          c = *p1;                              /* Pick up next character   */
          if(c==EOFCHAR)                        /* ^Z??                     */
          {                                     /* Yes,                     */
            fp->flags |= ATEOF;                 /*      Set EOF flag        */
            if(fp->offset > fp->hiwater)        /* set hiwater mark         */
                fp->hiwater = fp->offset;       /*     if necessary         */
            return(xbytes-bytes);               /* Return number read       */
          }                                     /****************************/
          else if (c == '\r')                   /* Carriage return?         */
          {                                     /*                          */
                p1++;                           /* Yes, just ignore that    */
                fp->offset++;                   /* Increment file offset    */
          }                                     /*                          */
          else                                  /****************************/
          {                                     /* Regular character        */
            *buff++ = c;                        /* Load buffer with byte    */
            bytes--;                            /* Decrement count          */
            p1++;                               /* increment counter        */
            fp->offset++;                       /* Increment file offset    */
          }                                     /****************************/
        }                                       /*                          */
    }                                           /* While bytes > 0          */

    if(fp->offset > fp->hiwater)                /* Fix up hiwater mark      */
        fp->hiwater = fp->offset;               /*                          */
    return(xbytes);                             /* Read fully satisfied     */
}                                               /****************************/
