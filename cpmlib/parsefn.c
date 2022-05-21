/***************************************************************************
 *                                                                          
 *                      _ p a r s e f n   F u n c t i o n                   
 *                      ---------------------------------                   
 *      Copyright 1983 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      Function "_parsefn()" is used to parse a filename into the CP/M fcb.
 *      NOTE: For PCDOS, an OS call is written in assembler ("_pc_parse()").
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = _parsefn(filnam,fcbp);                                
 *                                                                          
 *      Where:                                                              
 *              filnam  Is a null-terminated CP/M filename                  
 *              ret     Is NULL if entire string parsed,                 
 *                        -1 if there was an error, or                      
 *                        a pointer to the next item to parse               
 *              fcbp    Is the address of a CP/M File Control Block (fcb).  
 *                        Note: password will get stuffed into bytes 16-23  
 *                        user # (plus 1) in byte 24, if supplied in filnam.
 *      File Spec:                                                          
 *              #d:filename.typ;passwd                                      
 *                                                                          
 *      Modified:
 *      1/84 whf        handle 68K parsing error.
 *      3/84 sw         Fix off by 1 user # 
 ****************************************************************************/

#include <ctype.h>
#include <string.h>

#include "portab.h"
#include "osif.h"
#include "prototypes.h"

static void _strcpy(BYTE *s, BYTE *d, WORD c);
static BYTE *get_token(BYTE *src, BYTE *dest, WORD len);
static WORD stuff_drive(struct fcbtab *fcbp, BYTE *src);

    BYTE *                                      /****************************/
_parsefn(                                       /*                          */
    BYTE            *filnam,                    /* -> filename              */
    struct fcbtab       *fcbp)                  /* -> FCB address           */
{                                               /****************************/

        BYTE    tokbuf[32];                     /* Token temporary buffer   */
                                                /****************************/
        blkfill(fcbp,'\0',32);                  /* Init fcb area to NULLs   */
        blkfill(&(fcbp->fname[0]),' ',11);      /* with name area to spaces */
                                                /*                          */
        filnam = get_token(filnam,tokbuf,32);   /* Get the first token      */
        if(*filnam == ':')                      /* Is the delim a colon?    */
        {                                       /* If yes, then a drive spec*/
          if(stuff_drive(fcbp,tokbuf) == FAILURE) /* Check drive OK         */
                return FAILPTR;                 /* Return error if bad      */
          filnam=get_token(++filnam,tokbuf,32); /* Get next token           */
        }                                       /****************************/
                                                /*                          */
        if(*filnam == '*')                      /* Delimiter is a "*"??     */
        {                                       /*                          */
          blkfill(&(fcbp->fname[0]),'?',8);     /* Yes, fill with ?'s 1st   */
          filnam++;                             /* Bump to next character   */
        }                                       /****************************/
        _strcpy(tokbuf,fcbp->fname,8);          /* Copy the file name in    */
        if(*filnam == '.')                      /* If extension specified   */
        {                                       /*                          */
          filnam=get_token(++filnam,tokbuf,32); /* get extension token      */
          if(*filnam == '*')                    /* Is wildcarded ext?       */
          {                                     /*                          */
            blkfill(&(fcbp->fname[8]),'?',3);   /* If yes, fill with ?'s    */
            filnam++;                           /* Bump to next character   */
          }                                     /*                          */
          _strcpy(tokbuf,&(fcbp->fname[8]),3);  /* Copy the string          */
        }                                       /*                          */
        if(*filnam == ';')                      /* If password specified    */
        {                                       /*                          */
          filnam=get_token(++filnam,tokbuf,32); /* get extension token      */
          _strcpy(tokbuf,&(fcbp->fpasswd[0]),8);/* Copy the string          */
        }                                       /*                          */
        switch(*filnam)                         /* Check for EOS            */
        {                                       /*                          */
          case '\0':                            /* Null terminated          */
          case '\n':                            /* CCPM compatible          */
          case '\r':                            /*   "      "               */
                return NULL;                    /* everything a-ok          */
        }                                       /****                       */
        return FAILPTR;                         /* Not legal file name      */
}                                               /****************************/


static void _strcpy(                            /* Special string copy func */
        BYTE    *s,                             /* Source string            */
        BYTE    *d,                             /* Destination area         */
        WORD     c)                             /* Count                    */
{                                               /****************************/
        while ((*s) && (c))                     /*                          */
        {                                       /*                          */
                *d++ = toupper(*s);             /* Copy a byte              */
                s++;                            /* Up source                */
                c--;                            /* Down count               */
        }                                       /*                          */
}                                               /****************************/

/****************************************************************************/
/*                                                                          */
/*                  G e t _ t o k e n   R o u t i n e                       */
/*                  ---------------------------------                       */
/*                                                                          */
/*      Routine get_token takes the next token from the input string.       */
/*                                                                          */
/*      Calling Sequence:                                                   */
/*              ret = get_token(src,dest,len);                              */
/*                                                                          */
/*      Where:                                                              */
/*              src     is the address of the source string                 */
/*              dest    is the address of the destination area              */
/*              len     is the number of bytes in "dest".                   */
/*                                                                          */
/*              ret     is the returned address of the delimiter            */
/*                                                                          */
/****************************************************************************/
static BYTE *_delim="<>.,=:|[]*\n\r";           /* Delimiter set            */
static BYTE *get_token(                         /*                          */
        BYTE    *src,                           /* -> source                */
        BYTE    *dest,                          /* -> destination           */
        WORD     len)                           /* output area size         */
{                                               /****************************/
        while(*src && (!strchr(_delim,*src)) && len)/* Until done           */
        {                                       /*                          */
          *dest++ = *src++;                     /* Move 1 byte              */
           len--;                               /* Down count               */
        }                                       /****************************/
        *dest = '\0';                           /* Drop in null terminator  */
        return(src);                            /* Return delimiter addr.   */
}                                               /****************************/

/****************************************************************************/
/*                                                                          */
/*                 S t u f f _ d r i v e   F u n c t i o n                  */
/*                 ---------------------------------------                  */
/*                                                                          */
/*      Routine "stuff_drive" loads the user and drive code fields from     */
/*      the filename string.                                                */
/*                                                                          */
/*      Calling sequence:                                                   */
/*              ret = stuff_drive(fcbp,src);                                */
/*                                                                          */
/*      Where:                                                              */
/*              fcbp    Is the FCB address within the ccb                   */
/*              src     Is the source filename string                       */
/*              ret     Is SUCCESS if ok, FAILURE o.w.                      */
/*                                                                          */
/****************************************************************************/
static  WORD    stuff_drive(                    /*                          */
        struct fcbtab *fcbp,                    /* -> FCB area              */
        BYTE    *src)                           /* -> Source string         */
{                                               /****************************/
        WORD    i;                              /* Accumulator              */
        i = 0;                                  /* Zap to zero              */
        while(isdigit(*src))                    /* As long as source is dig.*/
        {                                       /*                          */
          i = (i*10) + *src++ - '0';            /* Convert to number        */
          fcbp->fuser = i+1;                    /* store user number        */
        }                                       /****************************/
                                                /*                          */
        if(*src)                                /* Drive code letter?       */
        {                                       /* Here => yes              */
          fcbp->drive = toupper(*src)-'A'+1;    /* get drive code byte      */
          src++;                                /* Increment source pointer */
        }                                       /*                          */
        if(*src ||                              /* Illegal  stuff?          */
                (fcbp->fuser > 16) ||           /*sw Bad user #?            */
                (fcbp->drive > 15) )            /* Bad drive?               */
                return(FAILURE);                /* Yes, quit                */
        return(SUCCESS);                        /* OK to continue           */
}                                               /****************************/
