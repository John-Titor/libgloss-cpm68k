/***************************************************************************
 *                                                                          
 *                      U n l i n k   F u n c t i o n                       
 *                      -----------------------------                       
 *      Copyright 1982 by Digital Research Inc.  All rights reserved.       
 *                                                                          
 *      The unlink function is used to delete a CP/M file by name.          
 *                                                                          
 *      Calling Sequence:                                                   
 *                                                                          
 *              ret = unlink(filename);                                     
 *                                                                          
 *      Where:                                                              
 *              filename        Is the null-terminated name of the file     
 *                                                                          
 *              ret             Is 0 for success, -1 for failure            
 *                                                                          
 ****************************************************************************/

#include <unistd.h>

#include "portab.h"
#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

int     unlink(                                 /* CLEAR FUNCTION ***********/
        const char *filename)                   /* -> filename              */
{                                               /****************************/
REG     WORD    ch;                             /* Channel number           */
REG     WORD    ret;                            /* Temp return value        */
                                                /****************************/
        if((ch=_allocc()) < 0)                  /* Allocate a channel       */
                return(FAILURE);                /* Can't                    */
                                                /*                          */
        __chinit(ch);                           /* Init fcb and ccb         */
        ret = __open(ch,(BYTE *)filename,DELETE); /* Delete the file        */
        _freec(ch);                             /* Free the channel         */
        return(ret);                            /* Return result of DELETE  */
}                                               /****************************/

