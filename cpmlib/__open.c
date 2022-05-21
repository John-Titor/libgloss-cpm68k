/***************************************************************************
 *                                                                          
 *                      _ _ o p e n   F u n c t i o n                       
 *                      -----------------------------                       
 *      Copyright 1982 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      Function "__open" is used to parse the CP/M fcb and open or create  
 *      the requested file.  Created files are deleted first, to avoid      
 *      directory problems.                                                 
 *                                                                          
 *      Calling Sequence:                                                   
 *                                                                          
 *              ret = __open(ch,filnam,bdosfunc);                           
 *                                                                          
 *      Where:                                                              
 *              ch      Is a vacant channel number                          
 *              filnam  Is a null-terminated CP/M filename                  
 *              bdosfunc Is the desired BDOS function to perform            
 *              ret     Is SUCCESS if everything ok, FAILURE o.w. unless:   
 *                        bdosfunc=SEARCHF/N, ret = dcnt (0-3)              
 *                                                                          
 *      This routine may also be used to delete files as well.              
 *                                                                          
 *      Modified  4-Nov-83 (whf) open files read-only if appropriate        
 *      Modified  8-Oct-83 (whf) handle user #s differently                 
 *      Modified  5-Oct-83 (whf) added '_parsefn()'                         
 *      Modified 11-Aug-83 (whf) for DRC                                    
 *      Modified 26-Jun-83 (sw) for wild cards and user # support           
 *                                                                          
 ****************************************************************************/

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

#define HIBIT 0x80

WORD    __open(                                 /****************************/
                                                /*                          */
        WORD            ch,                     /* Channel number           */
        const BYTE      *filnam,                /* -> filename              */
        WORD            bdosfunc)               /* BDOS Function            */
{                                               /****************************/
        FD      *fp;                            /* -> ccb area              */
        struct fcbtab   *fcbp;                  /* -> FCB area in ccb       */
        WORD    rv;                             /* Return value             */
        WORD    xuser;                          /* User number              */
                                                /****************************/
                                                /*                          */
        fp = _getccb(ch);                       /* Fetch ccb pointer        */
        fcbp = &(fp ->fcb);                     /* Fetch fcb pointer        */
        rv = SUCCESS;                           /* Default to success       */
                                                /****************************/
        if( _parsefn(filnam,fcbp) != NULL )     /* Parse filnam into fcb    */
                RETERR(FAILURE,EINVAL);         /* Quit if name not ok      */
        if(fcbp->fuser)                         /* User # specified?        */
                fp->user = fcbp->fuser;         /* put it where we use it   */
        if( bdosfunc == CREATE  &&              /* Creating file?           */
            index(fcbp->fname,'?'))             /* Wild cards @!@#$!!!      */
                RETERR(FAILURE,EINVAL);         /* Just quit                */
                                                /*                          */
        xuser = _chkuser(fp->user);             /* Change user # if needed  */
                                                /*                          */
        if(bdosfunc == CREATE)                  /* Creating file?           */
                __OSIF(DELETE,fcbp);            /*  delete it first         */
                                                /*                          */
        if(bdosfunc == OPEN)                    /* Opening a file?          */
        {                                       /* Yes...                   */
            if( fp->flags & ISREAD )            /* Open file Read-Only?     */
                (fcbp->fname)[5] |= HIBIT;      /* Turn on F6' attribute bit*/
        }                                       /*                          */
                                                /****************************/
        rv = __OSIF(bdosfunc,fcbp);             /* Do requested operation   */
                                                /****************************/
                                                /*                          */
        _uchkuser(fp->user,xuser);              /* Change back if needed    */
                                                /*                          */
        if(bdosfunc == SEARCHF || bdosfunc == SEARCHN) /*                   */
                return(rv);                     /* return directory count   */
        return((rv<=3) ? SUCCESS : FAILURE);    /* Binary return code       */
}                                               /****************************/
