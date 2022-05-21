/***************************************************************************
 *                                                                          
 *                         _ b l k i o   F u n c t i o n                    
 *                         -----------------------------                    
 *      Copyright 1982,1983 by Digital Research Inc.  All rights reserved.  
 *                                                                          
 *      The _blkio function is used to read / write a set of contiguous     
 *      sectors in an open file.                                            
 *                                                                          
 *      Edits:                                                              
 *              8-Dec-83 whf    handle PC-DOS changes.                      
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = _blkio(ccbp,sector,buffer,count,bdosfunc);            
 *                                                                          
 *      Where:                                                              
 *              ret     =  sectors actually read/written                    
 *              ccbp    -> the open file channel control block              
 *              sector  =  The desired starting sector number               
 *              buffer  =  The memory address to begin transfer             
 *              count   =  the number of sectors to read/write              
 *              bdosfunc=  The BDOS function number (B_READ / B_WRITE)      
 *                                                                          
 ****************************************************************************/

#include        "portab.h"
#include        "osif.h"
#include        "osiferr.h"
#include        "prototypes.h"

#define MIN(x,y) ( ((x) < (y)) ? (x) : (y) )

LONG
_blkio(
                                                /****************************/
             FD         *ccbp,                  /* open file pointer        */
             LONG       sector,                 /* Sector number            */
             BYTE       *buffer,                /* User's buffer area       */
             LONG       count,                  /* # of sectors to do       */
             int        bdosfunc)               /* BDOS function to use     */
{                                               /****************************/
        WORD    nsecs;                          /* # secs to xfer each time */
        LONG    seccnt;                         /* # sectors processed      */
        WORD    xuser;                          /* old user number          */
        WORD    retcode;                        /* return code from BDOS    */
                                                /****************************/
        seccnt = 0;                             /* Zero count initally      */
        xuser = _chkuser(ccbp->user);           /* Check for changed user # */
        nsecs = 1;                              /* default: xfer singly     */
                                                /*                          */
        while (count > 0)                       /* Until all sectors xferred*/
        {                                       /*                          */
                __OSIF(SETDMA,buffer);          /* Set new DMA address      */

#if HILO        /*==========================================================*/
                ccbp->fcb.record = sector;      /* Set new record number    */
#else           /*----------------------------------------------------------*/
                ccbp->fcb.record = sector << 8; /* Set new record number    */
#endif          /*==========================================================*/

                                                /****************************/
                retcode = __OSIF(bdosfunc,&(ccbp->fcb));/* do the read/write*/
                                                /****************************/
                if(retcode != 0)
                {                               /****************************/
                    nsecs = 0;                  /* ow. assume 0 secs read   */
                    seccnt += nsecs;            /* Return nfg               */
                    break;                      /* Return from bottom       */
                }                               /****************************/
                                                /*                          */
                sector += nsecs;                /* Increment sector number  */
                count  -= nsecs;                /* Down count               */
                seccnt += nsecs;                /* Bump sector count        */
                buffer += nsecs*SECSIZ;         /* Increment address        */
        }                                       /****************************/
        _uchkuser(ccbp->user,xuser);            /* Reset user # if needed   */
        return(seccnt);                         /* All is OK                */
}                                               /****************************/
