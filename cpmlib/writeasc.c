/***************************************************************************
 *                                                                          
 *                      W r t a s c    F u n c t i o n                      
 *                      ------------------------------                      
 *                                                                          
 *      Function "_wrtasc" is called from "write" to handle writes to a     
 *      file opend in ascii mode.                                           
 *                                                                          
 *      Calling sequence:                                                   
 *              ret = _wrtasc(fp,buffer,bytes);                             
 *                                                                          
 *      Where:                                                              
 *              fp      Points to the affected ccb.                         
 *              buffer  Is the buffer address                               
 *              bytes   Is the number of bytes to write                     
 *                                                                          
 *              ret     Is the number of bytes actually written             
 *                                                                          
 ****************************************************************************/

#include "portab.h"
#include "osif.h"
#include "osiferr.h"

#define OFFSECT   (fp->offset      & ~(SECSIZ-1))
#define HIWSECT   ((fp->hiwater-1) & ~(SECSIZ-1))


ssize_t _wrtasc(                                /****************************/
                                                /*                          */
        REG     FD      *fp,                    /* -> CCB                   */
        const   BYTE    *buff,                  /* -> User's buffer         */
        REG     size_t  bytes)                  /* # bytes to write         */
{                                               /****************************/
        REG     BYTE    *p1;                    /* Temp buffer pointer      */
        REG     BYTE    cc;                     /* character temp           */
        LOCAL   LONG    xsector;                /* Sector temp              */
//        LOCAL   LONG    nsector;                /* Multi-sector count       */
        LOCAL   LONG    written;                /* # bytes to write         */
        LOCAL   LONG    xbytes;                 /* Save byte count remaining*/
                                                /****************************/
        written = bytes;                        /* Remember original request*/
        cc = 0;                                 /* Init to garbage value    */
                                                /*                          */
while(bytes >0)                                 /* While more bytes to write*/
{                                               /*                          */
        xsector = fp->offset >> 7;              /* Compute beginning sector */
                                                /*                          */
        if(xsector != fp -> sector)             /* Have to read it first    */
        {                                       /*                          */
          if((fp->flags & DIRTY) != 0)          /* Buffer has data?         */
          {                                     /* Yes                      */
            if(_blkio(fp,fp->sector,            /*                          */
                fp->buffer,1L,B_WRITE) != 1)    /* Try to write             */
                        RETERR(FAILURE,EIO);    /* Couldn't                 */
            fp->flags &= ~DIRTY;                /* Nice clean buffer now    */
          }                                     /****************************/
                                                /*                          */
          if( OFFSECT <= HIWSECT)               /* and below hiwater mark   */
          {                                     /*                          */
              if(_blkio(fp,xsector,fp->buffer,  /*                          */
                  1L,B_READ) != 1)              /* Now read the correct sec */
                          RETERR(FAILURE,EIO);  /* Couldn't                 */
          }                                     /****************************/
          fp->sector = xsector;                 /* Mark buffer correctly    */
        }                                       /****************************/
                                                /*                          */
        p1 = &(fp->buffer[(int)fp->offset&(SECSIZ-1)]); /* 1st char to write*/
                                                /*                          */
        xbytes = bytes;                         /* Save byte count remaining*/
        while (p1 < &(fp->buffer[SECSIZ])       /* Until buffer is full     */
             && (bytes > 0))                    /* or until request done    */
        {                                       /****************************/
          if(*buff == '\n' && cc != '\r')       /* need to insert C/R       */
            cc = '\r';                          /*                          */
          else                                  /* Don't need to            */
          {                                     /*                          */
            cc = *buff++;                       /*                          */
            bytes--;                            /*                          */
          }                                     /****************************/
            *p1++ = cc;                         /* Put char in buffer       */
            fp->offset++;                       /* Increment file offset    */
        }                                       /****************************/
        if(p1 >= &(fp->buffer[SECSIZ]))         /* Need to write buffer     */
        {                                       /*                          */
          if(_blkio(fp,xsector,fp->buffer,      /*                          */
                1L,B_WRITE) !=1)                /* Try the write            */
            return(written-xbytes);             /* return # actually written*/
        }                                       /****************************/
        else                                    /*                          */
          fp -> flags |= DIRTY;                 /* Buffer dirty again       */
        if(bytes == 0)                          /* If done,                 */
        {                                       /* Check offset here        */
            if(fp->offset > fp->hiwater)        /*                          */
              fp->hiwater = fp->offset;         /* Fix hiwater if necessary */
            return(written);                    /*              return      */
        }                                       /*                          */
}                                               /****************************/
return written; // XXX keep compiler happy
}
