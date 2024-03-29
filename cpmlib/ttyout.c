/***************************************************************************
 *                                                                          
 *                      _ t t y o u t   F u n c t i o n                     
 *                      -------------------------------                     
 *                                                                          
 *      Function "_ttyout" is called to perform "write" operations to the   
 *      console device.                                                     
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = _ttyout(buffer);                                      
 *                                                                          
 *      Where:                                                              
 *              buffer  -> the output buffer (byte 0 is count)              
 *              ret     =  count on return                                  
 ****************************************************************************/
#include "portab.h"
#include "osif.h"
#include "prototypes.h"

ssize_t _ttyout(                                /***************************/
                                                /*                         */
        BYTE *buf)                              /* -> 1st char output      */
{                                               /***************************/
        WORD    ii;                             /* counter for '$' check   */
        WORD    count;                          /* =  # bytes to xfer      */
        BYTE    *cp;                            /* ptr for '$' check       */
                                                /*                         */
        count = *buf++;                         /* Get num bytes to output */
        buf[count] = '$';                       /* Terminate string        */
        for(ii=0, cp=buf; ii<count; ++ii)       /* Check for internal '$'  */
            if( buf[ii] == '$' )                /* Found one?              */
            {                                   /* Yes...                  */
                if( *cp != '$' )                /* If '$' not at start     */
                    __OSIF(C_WRITESTR,cp);      /* Print the string        */
                __OSIF(CONOUT,'$');             /* And output the '$'      */
                cp = &buf[ii+1];                /* Reset start of string   */
            }                                   /********                  */
        if( cp != &buf[ii] )                    /* Assuming we haven't yet */
                __OSIF(C_WRITESTR,cp);          /* Output the rest         */
                                                /*                         */
        return(count);                          /* return original count   */
}                                               /***************************/
