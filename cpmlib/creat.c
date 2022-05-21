/****************************************************************************
*
*                  C   F I L E   C R E A T E   R O U T I N E
*                  -----------------------------------------
*       Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*       The "creat" routine opens a new "C" file and returns a file id. 
*       Comes in 3 flavors: ascii (CP/M text files), binary, and default
*       (currently ascii).
*
*       Calling Sequence:
*               fid = creat(fname)
*               fid = creata(fname)
*               fid = creatb(fname)
*               fid = _creat(fname,type);
*
*       Where:
*
*               fname   is the address of a null terminated file name.
*               prot    is the UNIX file protection
*               type    is 0 for ASCII, 1 for BINARY
*
*****************************************************************************/
#include        "portab.h"                      /* Include std definitions  */
#include        "osif.h"                        /* Include CP/M definitions */
#include        "osiferr.h"
#include        "prototypes.h"

WORD    _creat (                                /****************************/
        const BYTE      *fname,                 /* -> File name             */
        WORD            type)                   /* ASCII/BINARY flag        */
{                                               /****************************/
        WORD ich;                               /* Channel number for open  */
        FD         *ch;                         /* -> CCB for channel       */
                                                /*                          */
        if((ich = _allocc()) == FAILURE)        /* Allocate a channel       */
                return (FAILURE);               /*      Can't (EMFILE)      */
                                                /*                          */
        __chinit(ich);                          /* Clear out channel's ccb  */
        ch = _getccb(ich);                      /* Get address of ccb       */
                                                /*                          */
        if(type==0)                             /* ASCII file?              */
                ch -> flags |= ISASCII;         /*      Yes, mark it.       */
                                                /****************************/
        if(_strcmp(fname,__tname) == 0)         /* Terminal file?           */
        {                                       /*                          */
                ch -> flags |= ISTTY|OPENED;    /* Set flag                 */
                return(ich);                    /* Return file descriptor   */
        }                                       /****************************/
        else if(_strcmp(fname,__lname) == 0)    /* List device?             */
        {                                       /*                          */
                ch -> flags |= ISLPT|OPENED;    /* set flag                 */
                return(ich);                    /*                          */
        }                                       /*                          */
                                                /****************************/
        if(__open(ich,fname,CREATE) != 0)       /* Use BDOS interface       */
                RETERR(FAILURE,ENOSPC);         /*   Oops, No dir space.    */
                                                /* Else:                    */
        ch -> flags |= OPENED;                  /*       Set OPEN bit       */
        return(ich);                            /*       Return Channel #   */
}                                               /****************************/

#if 0
WORD    creat(fname)                            /* CLEAR FUNCTION ***********/
    const BYTE *fname
{       return(_creat(fname,0));   }            /* default to ascii         */
#endif

WORD    creata(                                 /* CLEAR FUNCTION ***********/
    const BYTE *fname)
{       return(_creat(fname,0));   }            /* ascii file open          */

WORD    creatb(                                 /* CLEAR FUNCTION ***********/
    const BYTE *fname)
{       return(_creat(fname,1));   }            /* binary file open         */

