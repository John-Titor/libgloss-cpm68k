/*****************************************************************************
*
*               "__main()" Subroutine to "_main()"
*               ----------------------------------
*       Copyright 1983 by Digital Research Inc.  All rights reserved.
*
*       Routine "__main()" is entered from the "_main()" routine to start a 
*       C program.  The command string from CP/M is parsed into
*       a UNIX-like "argc/argv" setup, including simple I/O redirection.
*
*       This module can be compiled without wildcard parsing if the 
*       symbol "NOWILD" (note upper case) is defined in the compile line.
*
*       Edits:
*               28-Feb-84 whf  "_salloc()" goes to NOWILD region
*               02-Feb-84 whf  nowild -> NOWILD, removed write()
*               01-Jan-84 whf  add in handle for OPTION*.h
*               08-Dec-83 whf  handle PC-DOS diffs, add ">>" redirection
*               11-Oct-83 whf  converted to DRC from CPM68K
*
*       Calling Sequence:
*               return = _main(command,length);
*
*       Where:
*               command         Is the address of the command line from CP/M
*               length          Is the number of characters in the line, 
*                               excluding the termination character (CR/LF).
*
*****************************************************************************/
#if 0
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "portab.h"                             /* Include std definitions  */
#include "osif.h"                               /* Include CP/M Definitions */
#include "prototypes.h"

#define NOWILD // XXX TODO enable this, seems useful?

#define STDIN 0                                 /* file descriptors         */
#define STDOUT 1                                /*                          */
#define STDERR 2                                /*                          */

#define ISWHITE(ch) ((ch)=='\0' || isspace(ch)) /*                          */

MLOCAL  BYTE    __pname[] = "program";          /* default program name     */
MLOCAL  WORD    argc;                           /* Arg count                */
MLOCAL  BYTE    *argv;                          /* -> array of pointers     */
MLOCAL  BYTE    *argv2;                         /* Companion ptr to argv    */

MLOCAL VOID _err(char *s1, char *s2);
MLOCAL VOID addargv(REG BYTE *ptr);

VOID
__main(                                         /****************************/
        BYTE    *com,                           /* Command address          */
        WORD    len __unused)                   /* Command length           */ // XXX TODO remove?
{                                               /****************************/
REG     WORD     i;                             /* Define a count var.      */
REG     BYTE    *s;                             /* Temp byte pointer        */
REG     BYTE    *p;                             /* Another ""               */
REG     BYTE    c;                              /* Character temp           */
                                                /****************************/
        argv2 = argv = sbrk(0);                 /* -> first free location   */
        argc = 0;                               /* No args yet              */
        addargv(__pname);                       /*                          */
        for (s = com; *s; s += i)               /* One arg at a time        */
        {                                       /*                          */
                while (*s && isspace(*s))       /* Skip leading spaces      */
                        ++s;                    /*                          */
                if (!*s)                        /* End of line?             */
                        break;                  /* Yes, don't continue      */
                if(*s == '"' || *s == '\'')     /* Quoted string            */
                {                               /*                          */
                   c = *s;                      /* c = quote character      */
                    p = strchr(s+1,c);          /* Find next                */
                    if (p == NULLPTR)           /* Any?                     */
                        _err(s,": unmatched quote");/* No, bitch.           */
                    i = (WORD)(p-s);            /* Compute length           */
                    s[i++] = '\0';              /* Nullify at end           */
                    addargv(s+1);               /* Add to arg list          */
                }                               /****************************/
                else                            /*                          */
                {                               /****************************/
                 for(i = 0; !ISWHITE(s[i]); ++i)/* How many characters?     */
                                ;               /*                          */
                 if (s[i])                      /* If last is space, etc... */
                        s[i++] = '\0';          /* Make it a null for C     */
                 switch(*s)                     /* Now do i/o scan          */
                 {                              /****************************/
                  case '<':                     /* Redirecting input??      */
                    close(STDIN);               /* Yes, close TTY           */
                    if (opena(s + 1,READ) != STDIN) /* Open New             */
                    _err("Cannot open ", s + 1) ;/*  Can't ...              */
                    break;                      /* Done with this one       */
                                                /****************************/
                   case '>':                    /* Redirecting output??     */
                     close(STDOUT);             /* Close output             */
                     if(s[1]=='>') {            /* Appending?               */
                       if(opena(s+2,WRITE)!=STDOUT     /* Try to open old   */
                         || -1 == lseek(STDOUT,0L,2) ) /* At EOF            */
                       _err("Cannot append ",s+1);     /*   Can't           */
                     } else {                          /***                 */
                       if (creata(s+1,0)!=STDOUT)      /* Try to open new   */
                       _err("Cannot create ", s + 1);  /*   Can't           */
                     }                                 /*                   */
                     break;                     /****************************/
                                                /*                          */
                   default:                     /* Just a regular arg       */
                                                /****************************/
#ifndef NOWILD      /********************************************************/
                    /********************************************************/
                    if(strchr(s,'?') ||         /* Wild                     */
                       strchr(s,'*'))           /*      Cards?              */
                    {                           /****************************/
                      FD      *pfd;             /* File Desc temp           */
                      BYTE    tmpbuf[30];       /* Filename temp            */
                      pfd = _getccb(STDERR+1);  /* Use unused channel       */
                      __OSIF(SETDMA,pfd->buffer);/* Use buffer for DMA      */
                      c=__open(STDERR+1,s,SEARCHF); /* Do the search        */
                      if(c == 0xff)             /* Failed ...               */
                        _err(s,": No match");   /* Complain                 */
                      while(c != 0xff)          /* Do search next's         */
                      {                         /****************************/
                        _toasc(pfd,c,tmpbuf);   /* Convert file to ascii    */
                        p = _salloc(strlen(tmpbuf)+1);/* Allocate area     */
                        strcpy(p,tmpbuf);       /* Move in filename         */
                        addargv(p);             /* Add this file to argv    */
                        c=__open(STDERR+1,s,SEARCHN); /* Search next        */
                       }                        /****************************/
                    } else                      /*                          */
                    /********************************************************/
#endif              /********************************************************/

                        addargv(s);             /* save in argv             */
                 }                              /*                          */
                }                               /*                          */
        }                                       /****************************/
        addargv(NULLPTR);                       /* Insure terminator        */
        argc--;                                 /* Back off by 1            */
        if( brk(argv2)==FAILPTR)                /* Allocate the pointers    */
                _err("Stack Overflow","");      /* Call "Stack Overflow" rtn*/
        exit(main(argc,(const char **)argv,NULLPTR)); /* Invoke C program (No Env)*/
}
        
                                                /*                          */
MLOCAL VOID _err(                               /* Error routine            */
        char    *s1,                            /* Message text             */
        char    *s2)                            /* Filename                 */
{                                               /****************************/
        char buf[128];                          /* place to build message   */
                                                /*                          */
        strcpy(buf,s1);                         /* Output error message     */
        strcat(buf,s2);                         /* And filename             */
        strcat(buf,"\r\n$");                    /* + Newline                */
        __OSIF(C_WRITESTR,buf);                 /* output directly to CON:  */
        exit(-1);                               /* And fail hard            */
}                                               /****************************/

/*
 *      Addargv function -- adds a pointer to the argv array, getting the 
 *      space from the heap.
 */
MLOCAL VOID addargv(                            /****************************/
REG     BYTE    *ptr)                           /* -> Argument string to add*/
{                                               /*                          */
        *((BYTE **)argv2) = ptr;                /* Load pointer             */
        argv2 += sizeof(char *);                /* More room from heap      */
        argc++;                                 /* Increment arg count      */
}                                               /****************************/

/*****************************************************************************/
/*****************************************************************************/
#ifndef NOWILD
/*
 *      Toasc routine -- combines the FCB name in the DMA and the user number
 *      / drive field to produce an ascii file name for SEARCHes.
 *
 */
MLOCAL _toasc(p,c,buf)                          /*****************************/
REG     FD      *p;                             /* -> Data area              */
REG     BYTE     c;                             /* 0 .. 3 search code        */
REG     BYTE    *buf;                           /* Output buffer area        */
{                                               /*****************************/
REG     BYTE    *f;                             /* -> Fcb in DMA buffer      */
        WORD     i;                             /* Temp.                     */
                                                /*                           */
        *buf = '\0';                            /* Nullify at first          */
        i = FALSE;                              /*****************************/
        f = p->buffer;                          /* Pnt to results of search  */
#if CPM    /*================================================================*/
        f += c*32;                              /* c == directory search code*/
        if( p->user )                           /* User # not default?       */
        {                                       /* Yes                       */
                i = (p->user) -1;               /* Cvt to real user #        */
                if( i>=10 ) *buf++ = '1';       /* Assume user # <15         */
                *buf++ = '0' + (i % 10);        /* now for digits            */
                *buf++ = ':';                   /* finish off with this      */
                i = TRUE;                       /* remember                  */
        };                                      /*****************************/
#endif     /*================================================================*/
        if(p->fcb.drive)                        /* Drive specified?          */
        {                                       /* Yes                       */
          if(i)                                 /* User #?                   */
                buf--;                          /* Yes, back up over ':'     */
          *buf++ = p->fcb.drive - 1 + 'a';      /* Put in drive code         */
          *buf++ = ':';                         /* And delimiter             */
        }                                       /*                           */
        for(i=1;i<9;i++)                        /* Move the filename         */
        {                                       /*****************************/
           if(f[i] != ' ')                      /* Non-blank?                */
                *buf++ = tolower((f[i]&0x7f));  /* Yes, move it in           */
        }                                       /*****************************/
        *buf++ = '.';                           /* Put in delimiter          */
        for(i=9; i<12; i++)                     /* Move in extension         */
        {                                       /*                           */
           if(f[i] != ' ')                      /* Non-blank?                */
                *buf++ = tolower((f[i]&0x7f));  /* Yes, move it in           */
        }                                       /*****************************/
        *buf++ = '\0';                          /* Null at end               */
}                                               /*****************************/

#endif

/*****************************************************************************/
/*****************************************************************************/
#endif
