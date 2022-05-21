/***************************************************************************
 *                                                                          
 *                      _ l s t o u t   F u n c t i o n                     
 *                      -------------------------------                     
 *                                                                          
 *      Function "_lstout" is called to perform "write" operations to the   
 *      list device.                                                        
 *                                                                          
 *      Calling Sequence:                                                   
 *              ret = _lstout(buffer,count,func);                           
 *                                                                          
 *      Where:                                                              
 *              buffer  -> the output buffer (byte 0 is count)              
 *              ret     =  count on return                                  
 *                                                                          
 ****************************************************************************/

#include "portab.h"
#include "osif.h"
#include "prototypes.h"

ssize_t _lstout(                                /***************************/
        BYTE *buffer)                           /* -> 1st char output      */
{                                               /***************************/
        WORD    count;                          /* =  # bytes to xfer      */
        WORD    xcount;                         /* save area for count     */
                                                /*                         */
        xcount = count = (0xFF & *buffer++);    /* Copy for later          */
                                                /*                         */
        while(count-- > 0)                      /* Until all written       */
                __OSIF(LSTOUT,*buffer++);       /* Output next character   */
        return(xcount);                         /* return original count   */
}                                               /***************************/
