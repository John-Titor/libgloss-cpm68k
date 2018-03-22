/*****************************************************************************
*
*         C H A N N E L   A L L O C A T I N G   R O U T I N E S
*         -----------------------------------------------------
*       Copyright 1984 by Digital Research Inc.  All rights reserved.
*
*       Herein are all the routines which deal with Channel Control Block
*       (CCB) allocation and initialization.  These routines (documented
*       individually below) are:
*
*               i = _allocc();
*               _freec(i);
*               _chinit();
*               __chinit(fd)
*               ccbptr = _chkc(ch);
*
*****************************************************************************/
#include        "portab.h"                      /* Include std definitions  */
#include        "osif.h"                        /* Also CP/M ones           */
#include        "osiferr.h"                     /* To set error vars        */
#include        "prototypes.h"

#ifdef MAXF5                                    /*--------------------------*/
#  define       MAXCCBS 5                       /* Maximum Num CCBs         */
maxfiles5() { ; }                               /* stubroutine for option.h */
#else                                           /*--------------------------*/

#ifdef MAXF10                                   /*--------------------------*/
#  define       MAXCCBS 10                      /* Maximum Num CCBs         */
#else                                           /*--------------------------*/
#  define       MAXCCBS 16                      /* Maximum Num CCBs         */
#endif                                          /*--------------------------*/

#endif                                          /*--------------------------*/

LONG            _chvec  ={0};                   /* Allocate storage         */
FD              _fds[MAXCCBS];                  /* Allocate CCB storage     */



/*****************************************************************************
*               _ A L L O C C   /   _ F R E E C   R O U T I N E S
*               -------------------------------------------------
*       Routines "_allocc" and "_freec" are used to allocate / deallocate a 
*       channel number so that the user may do I/O directly without the OTS
*       getting in the way.
*       Note that this scheme preserves compatibility with OS's that really
*       truly use channels, and allows users to inform the C Run Time Library
*       routines that they are using the channel.
*       It's not important to use these routines under CP/M, since CP/M
*       allows you to have as many FCBs as you wish.
*
*       Calling Sequence:
*               i = _allocc();
*               _freec(i);
*****************************************************************************/

WORD    _allocc(void)                           /****************************/
{                                               /*                          */
        REG WORD i;                             /* Define 2 temporaries     */
        REG LONG j;                             /*                          */
                                                /*                          */
        j = 1;                                  /* Start with channel 0     */
        for(i=0;i<MAXCCBS;i++)                  /* Look at the bits         */
        {                                       /*                          */
          if((j & _chvec) == 0)                 /* If 0, then channel free  */
          {                                     /*                          */
                _chvec |= j;                    /* set allocated bit        */
                return(i);                      /* and return the channel # */
          }                                     /*                          */
          j <<= 1;                              /* Up to next bit           */
        }                                       /* End FOR loop             */
        RETERR(FAILURE,EMFILE);                 /* All channels in use!     */
}                                               /****************************/

WORD    _freec(                                 /****************************/
        WORD    ch)                             /* Channel number to free   */
{                                               /*                          */
        _chvec &= ~(1 << ch);                   /* Clear appropriate bit    */
        return(SUCCESS);                        /* Return OK                */
}                                               /****************************/

/*****************************************************************************
*                    C C B   I N I T I A L I Z A T I O N
*                    -----------------------------------
*       Routine "_chinit" is called from the run-time initialization to clear
*       out all the CCB storage.
*       Calling sequence:
*               _chinit();
*
*       Routine "__chinit(fd)" is called from other low-level routines
*       for single channel initialization.
*       Calling sequence:
*               __chinit(fd)
*       where:  fd = file descriptor #
*****************************************************************************/

void _chinit(void)                              /****************************/
{                                               /*                          */
        REG WORD        i;                      /*      Index               */
        for(i=0;i<MAXCCBS;i++)                  /*      For all channels    */
                __chinit(i);                    /*      Init fds(i);        */
}                                               /****************************/

/*****************************************************************************
*       The __chinit routine initializes only 1 channel.
*****************************************************************************/

void __chinit(int i)
{                                               /****************************/
        REG FD          *ch;                    /* -> CCB                   */

        ch =  _getccb(i);                       /* convert fd to CCB        */
        ch -> chan   = i;                       /* Load channel byte        */
        ch -> flags  = 0;                       /* clear flag word          */
        ch -> user   = 0;                       /* assume user 0            */
        ch -> sector = -1;                      /* Set no sector in buff    */
        ch -> offset = 0;                       /* Clear file offset word   */
        ch -> hiwater = 0;                      /* Init hiwater mark        */
        blkfill(&ch->fcb.drive,0,32);           /* Init FCB to 0's          */
        blkfill(&ch->fcb.fname[0],' ',11);      /* Init file name fields    */
}                                               /****************************/


/*****************************************************************************
*             C H A N N E L   N U M B E R   V A L I D A T I O N
*             -------------------------------------------------
*       This routine is used to validate a channel number and return the
*       pointer to the ccb area.  The channel must be in range and open.
*
*       Calling Sequence:
*
*       Where:
*               ch      Is the channel number
*               ccbptr  Is the returned ccb address, NULLFD if error
*****************************************************************************/

FD      *_chkc(                                 /****************************/
        int    ch)                              /* Facilitate error check   */
{                                               /*                          */
        REG     FD *xcb;                        /* -> CCB                   */
                                                /****************************/
        if(ch >= MAXCCBS)                       /* Is channel in range?     */
                RETERR(NULLFD,EBADF);           /* No, quit now.            */
                                                /*                          */
        xcb = _getccb(ch);                      /* xcb -> ccb for channel   */
        if((xcb->flags & OPENED) == 0)          /* Is channel OPEN?         */
                RETERR(NULLFD,EBADF);           /* Noooooooo!!              */
        return(xcb);                            /* Else, return pointer     */
}                                               /****************************/
