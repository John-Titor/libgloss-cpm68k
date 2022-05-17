/***************************************************************************
 *                                                                          
 *                      r e n a m e   F u n c t i o n                       
 *                      -----------------------------                       
 *      Copyright 1983 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      'rename()' changes a file's name.                                   
 *                                                                          
 *      Calling sequence:                                                   
 *              ret = rename(from,to)                                       
 *                                                                          
 *      Where:                                                              
 *              from -> a NULL terminated (existing) file name              
 *              to -> the NULL terminated new file name                     
 *              ret = SUCCESS if everything ok, FAILURE o.w.                
 *                                                                          
 ****************************************************************************/

#include <stdio.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

int     rename(                                 /* CLEAR FUNCTION ***********/
        const char *from,                       /* Existing file name       */
        const char *to)                         /* New file name            */
{
        BYTE fcbbuf[(sizeof(struct fcbtab))+16];/* Extra large fcb          */
  REG   struct fcbtab *fcbp;                    /* FCB pointer              */
  REG   WORD nuser;                             /* new user #               */
  REG   WORD xuser;                             /* system user #            */
        WORD rv;                                /* Return value             */

        fcbp = (struct fcbtab*)fcbbuf;          /* Point fcb to start       */
        if( FAILPTR == _parsefn((char *)from,fcbp)) /* Parse 'from' into fcb */
                RETERR(FAILURE,EINVAL);         /*    Bad filename          */
        nuser = fcbp->fuser;                    /* Save specified User #    */
        fcbp = (struct fcbtab*)&(fcbbuf[16]);   /* F_RENAME needs it        */
        if( FAILPTR == _parsefn((char *)to,fcbp)) /* Parse 'to' to fcb[16]  */
                RETERR(FAILURE,EINVAL);         /*    Bad filename          */
        if( fcbp->fuser )                       /* user # specified?        */
        {                                       /*                          */
                if(nuser && nuser!=fcbp->fuser) /* Differing User #s?       */
                        RETERR(FAILURE,EINVAL); /*   that's a no-no         */
                nuser = fcbp->fuser;            /* Use this user #          */
        }                                       /****                       */
        xuser = _chkuser(nuser);                /* Change user # if needed  */
                                                /****************************/
        rv = __OSIF(F_RENAME,fcbbuf);           /* Use special fcb          */
                                                /****************************/
        _uchkuser(nuser,xuser);                 /* Change back if needed    */
        if( rv != 0 )                           /* Did it work?             */
                RETERR(FAILURE,ENOENT);         /*   no...                  */
        return SUCCESS;                         /* It worked!               */
}                                               /****************************/
