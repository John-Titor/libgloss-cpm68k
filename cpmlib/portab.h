/****************************************************************************
*
*         C P / M   C   R U N   T I M E   L I B   H E A D E R   F I L E
*         -------------------------------------------------------------
*       Copyright 1982 by Digital Research Inc.  All rights reserved.
*
*       This is an include file for assisting the user to write portable
*       programs for C.  All processor dependencies should be located here.
*
****************************************************************************/
/*
 *      Standard type definitions
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define BYTE    int8_t                          /* Signed byte             */
#define UBYTE   uint8_t                         /* Unsigned byte           */
#define BOOLEAN bool                            /* 2 valued (true/false)   */
#define WORD    int16_t                         /* Signed word (16 bits)   */
#define UWORD   uint16_t                        /* unsigned word           */

#define LONG    int32_t                         /* signed long (32 bits)   */
#define ULONG   uint32_t                        /* Unsigned long           */

#define REG     register                        /* register variable       */
#define LOCAL   auto                            /* Local var on 68000      */
#define EXTERN  extern                          /* External variable       */
#define MLOCAL  static                          /* Local to module         */
#define GLOBAL  /**/                            /* Global variable         */
#define VOID    /**/                            /* Void function return    */
#define DEFAULT int16_t                         /* Default size            */
#define FLOAT   float                           /* Floating Point          */
#define DOUBLE  double                          /* Double precision        */


/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define FAILURE (-1)                    /*      Function failure return val */
#define SUCCESS (0)                     /*      Function success return val */
#define YES     true                    /*      "TRUE"                      */
#define NO      false                   /*      "FALSE"                     */
#define FOREVER for(;;)                 /*      Infinite loop declaration   */
#if 0
#define NULL    0                       /*      Null character value        */
#define NULLPTR (char *) 0              /*      Null pointer value          */
#endif
#define EOF     (-1)                    /*      EOF Value                   */
#define TRUE    true                    /*      Function TRUE  value        */
#define FALSE   false                   /*      Function FALSE value        */

#include <string.h>
#define blkfill(ptr, val, len)  memset((void *)ptr, val, len)
#define blkmove(to, from, len)  memmove((void *)to, (void *)from, len)

#include <assert.h>
#define ASSERT(expr)            assert(expr)
/****************************************************************************/
/****************************************************************************/

