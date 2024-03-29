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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*
 *      Standard type definitions
 */
#define BYTE    int8_t                          /* Signed byte             */
#define UBYTE   uint8_t                         /* Unsigned byte           */
#define WORD    int16_t                         /* Signed word (16 bits)   */
#define UWORD   uint16_t                        /* unsigned word           */

#define LONG    int32_t                         /* signed long (32 bits)   */
#define ULONG   uint32_t                        /* Unsigned long           */

#define FLOAT   float                           /* Floating Point          */
#define DOUBLE  double                          /* Double precision        */


/****************************************************************************/
/*      Miscellaneous Definitions:                                          */
/****************************************************************************/
#define FAILURE (-1)                    /*      Function failure return val */
#define FAILPTR ((BYTE *)(-1))          /*      FAILURE                  */
#define SUCCESS (0)                     /*      Function success return val */
#define EOF     (-1)                    /*      EOF Value                   */

#include <string.h>
#define blkfill(ptr, val, len)  memset((void *)ptr, val, len)
#define blkmove(to, from, len)  memmove((void *)to, (void *)from, len)
#define _strcmp(s1, s2)		    strcasecmp(s1, s2)

/****************************************************************************/
/****************************************************************************/

