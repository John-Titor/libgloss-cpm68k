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
*               fid = creat(fname,prot)
*               fid = creata(fname,prot)
*               fid = creatb(fname,prot)
*               fid = _creat(fname,prot,type);
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
        BYTE    *fname,                         /* -> File name             */
        WORD    prot __unused,                  /* Open mode                */ // XXX TODO remove
        WORD    type)                           /* ASCII/BINARY flag        */
{                                               /****************************/
        REG  WORD ich;                          /* Channel number for open  */
        REG  FD         *ch;                    /* -> CCB for channel       */
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
WORD    creat(fname,prot)                       /* CLEAR FUNCTION ***********/
    BYTE *fname; WORD prot;
{       return(_creat(fname,prot,0));   }       /* default to ascii         */
#endif

WORD    creata(                                 /* CLEAR FUNCTION ***********/
    BYTE *fname, WORD prot)
{       return(_creat(fname,prot,0));   }       /* ascii file open          */

WORD    creatb(                                 /* CLEAR FUNCTION ***********/
    BYTE *fname, WORD prot)
{       return(_creat(fname,prot,1));   }       /* binary file open         */

