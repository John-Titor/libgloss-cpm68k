/***************************************************************************
 *                                                                          
 *                      W r t b i n    F u n c t i o n                      
 *                      ------------------------------                      
 *      Copyright 1984, Digital Research Inc.  All rights reserved.
 *                                                                          
 *      Function "wrtbin" is called from "write" to take advantage of a     
 *      potential multi-sector transfer for binary files.                   
 *
 *      Completely re-written 7 Feb 1984 whf.
 *                                                                          
 *      Calling sequence:                                                   
 *              ret = _wrtbin(fp,buffer,bytes);                             
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
#include "prototypes.h"

#define NDEBUG
#include <assert.h>

#define OFFSECT   (fp->offset      & ~(SECSIZ-1))
#define HIWSECT   ((fp->hiwater-1) & ~(SECSIZ-1))


ssize_t _wrtbin(                                /****************************/
                                                /*                          */
        FD      *fp,                            /* -> CCB                   */
        const BYTE *buff,                       /* -> User's buffer         */
        size_t  bytes)                          /* # bytes to write         */
{                                               /****************************/

        UWORD   nbs;                            /* counter                  */
        UWORD   xsector;                        /* Current sector           */
        UWORD   nsector;                        /* Multi-sector count       */
        UWORD   written;                        /* # bytes to written       */
        UWORD   BufPos;                         /* Position in buffer       */
                                                /****************************/
                                                /****************************/
        written = 0;                            /* Num written so far       */
                                                /* Get buffer cleaned and   */
                                                /*   current:               */
        xsector = fp->offset >> 7;              /* Calculate present sector */
        if(xsector != fp->sector) {             /* Do sectors match?        */
            if(fp->flags & DIRTY)               /* No, is buffer dirty?     */
                if(_blkio(fp,fp->sector,fp->buffer,1L,B_WRITE) != 1) /*     */
                    RETERR(FAILURE,EIO);        /* Couldn't write buffer    */
            if( OFFSECT > HIWSECT)              /* Within the hiwater area? */
                 blkfill(fp->buffer,0,SECSIZ);  /* No: Zero out the buffer  */
            else if(_blkio(fp,(LONG)xsector,fp->buffer,1L,B_READ) != 1)
                 RETERR(FAILURE,EIO);           /* Can't preread buffer     */
            fp->flags &= ~DIRTY;                /* Clear dirty bit          */
            fp->sector = xsector;               /* Buffer labelled          */
        }                                       /****************************/
                                                /****************************/
                                                /* Xfer data to buffer until*/
                                                /* 1) end of data,          */
                                                /* 2) end of buffer, or     */
                                                /* 3) sector aligned multi- */
                                                /*    sector write.         */
        BufPos = fp->offset & (SECSIZ-1);       /* Where are we in buffer?  */
        nbs = SECSIZ - BufPos;                  /* Room left in buffer      */
        if( nbs > bytes )                       /* More room than we need?  */
             nbs = bytes;                       /*   Adjust to bytes to xfer*/
        else if( nbs == SECSIZ )                /* Sector aligned multi-sec?*/
             nbs = 0;                           /*   Don't buffer now.      */
        if( nbs > 0 ) {                         /* Any room/data to buffer? */
            blkmove(fp->buffer+BufPos,buff,nbs);/* Move into write buffer   */
            written += nbs;                     /* Indicate we've written   */
            fp->offset += nbs;                  /*     "      "      "      */
            buff += nbs;                        /* Ground we've covered     */
            bytes -= nbs;                       /* num bytes left to write  */
            if( BufPos + nbs != SECSIZ ) {      /* Buffer full?             */
                fp->flags |= DIRTY;             /* No, mark it              */
                fp->sector = xsector;           /* Label buffer             */
            } else {                            /*                          */
                if(_blkio(fp,(LONG)xsector,fp->buffer,1L,B_WRITE) != 1) 
                    RETERR(FAILURE,EIO);        /* Couldn't                 */
                fp->flags &= ~DIRTY;            /* turn off dirty bit       */
                fp->sector = -1;                /* buffer clean             */
                xsector++;                      /* Bump sector counter      */
            }                                   /****************************/
        }                                       /****************************/
                                                /*                          */
                                                /****************************/
        if( bytes >= SECSIZ ) {                 /* Can we do multisector io?*/
            nsector = bytes >> 7;               /* divide by 128 for sectors*/
            if(_blkio(fp,(LONG)xsector,(BYTE *)buff,(LONG)nsector,B_WRITE) != nsector)
                                                /* Multi-sector xfer        */
                RETERR(FAILURE,EIO);            /* Just quit on error       */
            xsector += nsector;                 /* Update current sector    */
            nbs = nsector << 7;                 /* number bytes xferred     */
            written += nbs;                     /* Indicate we've written   */
            fp->offset += nbs;                  /*     "      "      "      */
            buff += nbs;                        /* Ground we've covered     */
            bytes -= nbs;                       /* num bytes left to write  */
        }                                       /****************************/
                                                /****************************/
        if( bytes > 0 ) {                       /* Any room/data to buffer? */
            assert( fp->flags & DIRTY  == 0 );  /* Buffer should be clean   */
            if( OFFSECT > HIWSECT)              /* Within the hiwater area? */
                 blkfill(fp->buffer,0,SECSIZ);  /* No: Zero out the buffer  */
            else if(_blkio(fp,(LONG)xsector,fp->buffer,1L,B_READ) != 1) 
                 RETERR(FAILURE,EIO);           /* Can't preread buffer     */
            assert( bytes < SECSIZ );           /* Should be cut to size    */
            blkmove(fp->buffer,buff,bytes);     /* Move into write buffer   */
            fp->flags |= DIRTY;                 /* Mark buffer              */
            fp->sector = xsector;               /*   and label              */
            written += bytes;                   /* Indicate we've written   */
            fp->offset += bytes;                /*     "      "      "      */
            buff += bytes;                      /* Ground we've covered     */
        }                                       /****************************/
                                                /*                          */
        if(fp->offset > fp->hiwater)            /* See if above hiwater mark*/
                fp->hiwater = fp->offset;       /* Fix if necessary         */
        return written;                         /* Return requested #       */
}                                               /****************************/
