/***************************************************************************
 *                                                                          
 *                                 O S I F . H                              
 *                                 -----------                              
 *      Copyright 1982,1983 by Digital Research Inc. All rights reserved.   
 *                                                                          
 *      Edits:                                                              
 *      7-Mar-84  sw    Fix for 68K
 *      28-Feb-84 whf   Add C_STAT for ttyinraw()
 *      5-Jan-84 whf    Moved MAXCCBS to channel.c                          
 *      29-Dec-83 whf   Added F_TRUNC for tclose()                          
 *      12-Dec-83 whf   Change from "CPM.H" to "OSIF.H"                     
 *      9-Dec-83 whf    Handle PCDOS differences                            
 *      3-Nov-83 whf    Add multi-sector i/o                                
 *      19-Oct-83 whf   Add QUEUE handling info                             
 *      6-Oct-83 whf    Redefine reserved area in fcb for parsefn() return  
 *      25-June-83 sw   Add user number to file "fd" structure.             
 *                                                                          
 *      This file contains O.S. specific definitions for the                
 *      DRI CLEAR/C Run Time Library.                                       
 *      This file is intended only for inclusion with those functions       
 *      dealing directly with the O.S. interface, as well as any function   
 *      which has hardware dependent code (byte storage order, for          
 *      instance).                                                          
 *                                                                          
 ****************************************************************************/

#pragma once

#define CPM     1       /* CP/M version 2.2 */
#define MC68000 1       /* Motorola 68000 */
#define CPM68K  1       /* CP/M 68000 ver 2.2 */

#include "portab.h"


/****************************************************************************
 *      CP/M FCB definition
 ****************************************************************************/
struct  fcbtab                                  /****************************/
{                                               /*                          */
        BYTE    drive;                          /* Disk drive field [0]     */
        BYTE    fname[8];                       /* File name [1-8]          */
        BYTE    ftype[3];                       /* File type [9-11]         */
        BYTE    extent;                         /* Current extent number[12]*/
        BYTE    s1,s2;                          /* "system reserved" [13-14]*/
        BYTE    reccnt;                         /* Record counter [15]      */
        BYTE    fpasswd[8];                     /* Parsefn passwd area[16-23]*/
        BYTE    fuser;                          /* Parsefn user# area [24]  */
        BYTE    resvd[7];                       /* More "system reserved"   */
        LONG    record;                         /* Note -- we overlap [32-36]*/
                                                /* current record field to  */
                                                /* make this useful.        */
};                                              /****************************/
                                                /*                          */
#define SECSIZ          128                     /* size of CP/M sector      */
                                                /*   to obtain nsecs on err */
                                                /****************************/

/***************************************************************************
 *                                                                          
 *      Channel Control Block (CCB)                                         
 *                                                                          
 *      One CCB is allocated (statically) for each of the 16 possible open  
 *      files under C (including STDIN, STDOUT, STDERR).  Permanent data    
 *      regarding the channel is kept here.                                 
 *                                                                          
 *                                                                          
 ****************************************************************************/

struct  ccb                             /************************************/
{                                       /*                                  */
        WORD    flags;                  /*sw    Flags byte                  */
        BYTE    user;                   /*sw    User #                      */
        BYTE    chan;                   /*      Channel number being used   */
        LONG    offset;                 /*      File offset word (bytes)    */
        LONG    sector;                 /*      Sector currently in buffer  */
        LONG    hiwater;                /*      High water mark             */
        struct fcbtab fcb;              /*      File FCB (may have TTY info)*/
        BYTE    buffer[SECSIZ];         /*      Read/write buffer           */
};                                      /************************************/

extern  struct  ccb     _fds[]; /*  */  /*      Declare storage             */
#define FD struct ccb                   /*      FD Type definition          */
#define NULLFD ((FD *)0)                /*      NULLPTR for FD              */
                                        /************************************/
/*      Flags word bit definitions                                          */
                                        /************************************/
#define OPENED  0x01                    /*      Channel is OPEN             */
#define ISTTY   0x02                    /*      Channel open to TTT         */
#define ISLPT   0x04                    /*      Channel open to LPT         */
#define ISREAD  0x08                    /*      Channel open readonly       */
#define ISASCII 0x10                    /*      ASCII file attached         */
#define ATEOF   0x20                    /*      End of file encountered     */
#define DIRTY   0x40                    /*      Buffer needs writing        */
#define ISSPTTY 0x80                    /*      Special tty info            */
#define ISAUX   0x100                   /*sw    Auxiliary device            */
#define ISQUE   0x0200                  /*whf   Queue device                */
                                        /************************************/
#define READ    0                       /* Read mode parameter for open     */
#define WRITE   1                       /* Write mode                       */

/*      CCB manipulation macros         *************************************/
#define _getccb(i) (&_fds[i])           /*      Get CCB addr                */


/***************************************************************************
 *                                                                          
 *              O S I F   F u n c t i o n   D e f i n i t i o n s           
 *              -------------------------------------------------           
 *                                                                          
 *      Following are OSIF function definitions used by the C runtime       
 *      library.                                                            
 *                                                                          
 ****************************************************************************/

extern UWORD __BDOS(UWORD fn, ULONG arg);

                                                /****************************/
#define __OSIF(fn,arg) __BDOS((fn),(ULONG)(arg)) /* CPM68K does it this way  */
#define EXIT     0                              /* Exit to BDOS             */
#define CONIN    1                              /* direct echoing con input */
#define CONOUT   2                              /* Direct console output    */
#define LSTOUT   5                              /* Direct list device output*/
#define CONIO    6                              /* Direct console I/O       */
#define C_WRITESTR      9                       /* Console string output    */
#define CONBUF  10                              /* Read console buffer      */
#define C_STAT          11                      /* Get console status       */
#define S_BDOSVER       12                      /* Get System BDOS Ver Num  */
#define OPEN    15                              /* OPEN a disk file         */
#define CLOSE   16                              /* Close a disk file        */
#define SEARCHF 17                              /* Search for first         */
#define SEARCHN 18                              /* Search for next          */
#define DELETE  19                              /* Delete a disk file       */
#define CREATE  22                              /* Create a disk file       */
#define F_RENAME        23                      /* Rename a disk file       */
#define SETDMA  26                              /* Set DMA address          */
#define USER    32                              /*sw Get / set user number  */
#define B_READ  33                              /* Read Random record       */
#define B_WRITE 34                              /* Write Random record      */
#define FILSIZ  35                              /* Compute File Size        */
#define F_MULTISEC      44                      /* Set Multi-Sector Count   */
#define P_CHAIN         47                      /* Program Chain            */
#define SETVEC  61                              /* Set exception vector     */
#define N_NETSTAT       68                      /* Get Network Status       */
#define F_TRUNC 99                              /* Truncate file function   */
#define S_OSVER         163                     /* Get OS Version Number    */
/****************************************************************************/
/* Other CP/M definitions                                                   */
/****************************************************************************/
#define TERM    "CON:"                          /* Console file name        */
#define LIST    "LST:"                          /* List device file name    */
#define EOFCHAR 0x1a                            /* End of file character-^Z */
                                                /****************************/

/****************************************************************************/
/*      Hardware dependencies                                               */
/****************************************************************************/
                                                /****************************/
#if MC68000 | Z8000                             /* 68K or Z8000             */
#define HILO 1                                  /* used when bytes stored   */
#else                                           /*                          */
#define HILO 0                                  /* used when bytes stored   */
#endif                                          /*                          */
                                                /*                          */
#if HILO                                        /* Hi/Lo storage used in    */
struct long_struct{                             /*   68K                    */
        BYTE lbhihi;                            /* Use this for accessing   */
        BYTE lbhilo;                            /*   ordered bytes in 32 bit*/
        BYTE lblohi;                            /*   LONG qtys.             */
        BYTE lblolo;                            /*                          */
};                                              /*                          */
struct word_struct{                             /* Use this for accessing   */
        WORD lwhi;                              /*   ordered words in 32 bit*/
        WORD lwlo;                              /*   LONG qtys.             */
};                                              /*                          */
#else                                           /****************************/
struct long_struct{                             /* Lo/Hi storage use on     */
        BYTE lblolo;                            /*   PDP-11, VAX, 8086,...  */
        BYTE lblohi;                            /*                          */
        BYTE lbhilo;                            /*                          */
        BYTE lbhihi;                            /*                          */
};                                              /*                          */
struct word_struct{                             /*                          */
        WORD lwlo;                              /*                          */
        WORD lwhi;                              /*                          */
};                                              /*                          */
#endif                                          /****************************/

/*************************** end of osif.h **********************************/
