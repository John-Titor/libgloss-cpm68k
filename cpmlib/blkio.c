/***************************************************************************
 *                                                                          
 *                         _ b l k i o   F u n c t i o n                    
 *                         -----------------------------                    
 *      Copyright 1982,1983 by Digital Research Inc.  All rights reserved.  
 *                                                                          
 *      The _blkio function is used to read / write a set of contiguous     
 *      sectors in an open file.                                            
 *      Note: if CPM, it will use CP/M function 44 (Set Multi-Sector Count) 
 *      if os_multisectorio is TRUE (BDOS versions 3.0 and above).          
 *      Note 2: if PC-DOS, it will use block read and write functions.      
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
#include        "osattr.h"
#include        "prototypes.h"

#define MIN(x,y) ( ((x) < (y)) ? (x) : (y) )

static int __cpmrv = 0; // never set / used XXX TODO remove

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
        WORD    Used_Multisec;                  /* flag for resetting msecio*/
                                                /****************************/
        seccnt = 0;                             /* Zero count initally      */
        Used_Multisec = 0;                      /* Haven't used it yet      */
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

                                                /*   Multi Sector I/O xfer? */
                if( (count > 1  &&  os_multisectorio ) || nsecs > 1 ) 
                {                               /* yes!                     */
                    if( os_oldmultisec )
                         nsecs = MIN(16,count); /* Older version of msecio  */
                    else nsecs = MIN(128,count);/* Later version of msecio  */
                    ++Used_Multisec;            /* Flag: need to reset      */
                    __OSIF(F_MULTISEC,nsecs);   /* make the call            */
                }                               /*******                    */
                                                /****************************/
                retcode = __OSIF(bdosfunc,&(ccbp->fcb));/* do the read/write*/
                                                /****************************/
                if(retcode != 0)
                {                               /****************************/
                    if(nsecs>1)                 /* were we trying multi-sec?*/
                    {                           /* yes! do error handling   */
                      if( os_oldmultisec )      /* Older msecio?            */
                         nsecs = __cpmrv >> 12; /* adjust nsecs read like so*/
                      else nsecs = __cpmrv >> 8;/* later versions: use AH   */
                    }                           /***                        */
                    else nsecs = 0;             /* ow. assume 0 secs read   */
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
        if( Used_Multisec )                     /* Need to reset msecio?    */
            __OSIF(F_MULTISEC,1);               /*   then do so...          */
        return(seccnt);                         /* All is OK                */
}                                               /****************************/
