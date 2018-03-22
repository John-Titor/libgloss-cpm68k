/***************************************************************************
 *                                                                          
 *                        R d b i n   F u n c t i o n                       
 *                        ---------------------------                       
 *                                                                          
 *      The "_rdbin" function performs a read operation for binary files.   
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = _rdbin(fp,buffer,bytes);                              
 *                                                                          
 *      Where:                                                              
 *              fp      -> to the open CCB                                  
 *              buffer  -> the user's buffer                                
 *              bytes   =  the number of bytes to be read                   
 *              ret     =  the number of bytes actually read                
 *                                                                          
 ****************************************************************************/

#include "portab.h"
#include "osif.h"
#include "osiferr.h"

ssize_t _rdbin(                                 /****************************/
                                                /*                          */
REG     FD      *fp,                            /* -> ccb for the file      */
REG     BYTE    *buff,                          /* -> buffer to receive data*/
REG     size_t  bytes)                          /* =  # bytes to xfer       */
{                                               /****************************/

REG     BYTE    *p;                             /* Temp byte pointer        */
LOCAL   LONG    xsector;                        /* Temp sector number       */
LOCAL   LONG    nsector;                        /* Multi-sector count       */
LOCAL   LONG    xbytes;                         /* byte count temp          */
REG     LONG    i;                              /* Temp index               */
                                                /****************************/
        xbytes = bytes;                         /* Preserve org byte cnt    */
        xsector = fp->offset >> 7;              /* Calculate starting sector*/
                                                /*                          */
        if((fp->offset & (SECSIZ-1)) != 0)      /* If not on boundary,      */
        {                                       /*      must buffer         */
          if(fp->sector != xsector)             /* Do sectors match?        */
          {                                     /*                          */
            if((fp->flags & DIRTY) != 0)        /* Buffer dirty?            */
            {                                   /*                          */
              if(_blkio(fp,fp->sector,          /*                          */
                fp->buffer,1L,B_WRITE) != 1)    /* Try to write old buffer  */
                        RETERR(FAILURE,EIO);    /* Can't; just quit.        */
              fp->flags &= ~DIRTY;              /* not dirty anymore        */
            }                                   /****************************/
            if(_blkio(fp,xsector,fp->buffer,    /* Now read the             */
                1L,B_READ) != 1)                /*              next sector */

                    RETERR(FAILURE,EIO);        /* Must be EOF              */
            fp->sector = xsector;               /* Mark buffer              */
          }                                     /****************************/
          p = &fp->buffer[(int)fp->offset&(SECSIZ-1)];/* buffer offset      */
                                                /*                          */
          while(p < &(fp->buffer[SECSIZ]) &&    /* Move the bytes           */
                bytes > 0)                      /*                          */
          {                                     /*                          */
                *buff++ = *p++;                 /* Copy data byte           */
                bytes--;                        /* Decrement byte counter   */
                fp->offset++;                   /* Increment offset         */
          }                                     /****************************/
          if(bytes == 0)                        /* Zero byte count now?     */
          {                                     /*                          */
                if(fp->offset > fp->hiwater)    /* Adjust hiwater if        */
                        fp->hiwater=fp->offset; /*              necessary   */
                return(xbytes);                 /* Yes, just return         */
          }                                     /*                          */
          xsector++;                            /* Bump sector pointer      */
        }                                       /****************************/

/***************************************************************************
 * At this point, a multi-sector transfer may be accomplished.              */

/****************************************************************************/
                                                /*                          */
        i = 0;                                  /* In case no xfer          */
        nsector = bytes >> 7;                   /* Compute number of sectors*/
        if(nsector > 0)                         /* need to transfer??       */
           i=_blkio(fp,xsector,buff,nsector,    /* Do the                   */
                B_READ);                        /*         transfer         */
                                                /*                          */
        xsector += i;                           /* Update sector counter    */
        fp->offset += i*SECSIZ;                 /* Update offset            */
        bytes  -= i*SECSIZ;                     /* Update byte count        */
        buff   += i*SECSIZ;                     /* Update buffer address    */
                                                /*                          */
        if(fp->offset > fp->hiwater)            /* Adjust hiwater           */
                fp->hiwater = fp->offset;       /*                if needed */
        if (i != nsector)                       /* Short read??             */
        {                                       /*                          */
          fp->flags |= ATEOF;                   /* set EOF Flag             */
          return(xbytes - bytes);               /* Return actual read count */
        }                                       /****************************/
        if(bytes == 0)                          /* Done?                    */
                return(xbytes);                 /*       Yes, return        */
                                                /****************************/
        if((fp->flags & DIRTY) != 0)            /* Buffer dirty?            */
        {                                       /* This can be true for     */
              if(_blkio(fp,fp->sector,          /*  boundary conditions     */
                fp->buffer,1L,B_WRITE) != 1)    /* Try to write old buffer  */
                        RETERR(FAILURE,EIO);    /* Can't; just quit.        */
              fp->flags &= ~DIRTY;              /* not dirty anymore        */
        }                                       /****************************/
        if(_blkio(fp,xsector,fp->buffer,1L,     /* Read into buffer         */
                B_READ) != 1)                   /*                          */
        {                                       /*                          */
                fp->flags |= ATEOF;             /* End of file              */
                return(xbytes-bytes);           /* return corrected count   */
        }                                       /****************************/
                                                /*                          */
        fp->sector = xsector;                   /* Update data area         */
        fp->offset += bytes;                    /* Increment offset         */
        p = &fp->buffer[0];                     /* Start with 1st byte      */
        while (bytes > 0)                       /* Count down               */
        {                                       /*              bytes       */
                *buff++ = *p++;                 /* Move a byte              */
                bytes--;                        /*      Down count          */
        }                                       /****************************/

        if(fp->offset > fp->hiwater)            /* Fix up hiwater mark      */
                fp->hiwater = fp->offset;       /*                          */
        return(xbytes);                         /* Read fully satisfied     */
}                                               /****************************/
