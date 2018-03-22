//
// CP/M pre-main code
//

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "prototypes.h"
#include "osif.h"

BYTE *__tname = "CON:"; // XXX TODO constify
BYTE *__lname = "LST:";

int
_main(void)
{
    _chinit();                              /* Initialize channels      */
    _open(__tname,READ,0);                  /* Open STDIN               */
    _open(__tname,WRITE,0);                 /* Open STDOUT              */
    _open(__tname,WRITE,0);                 /* Open STDERR              */

    // XXX ctors
    // XXX argv / argc

    return main(0, NULL, NULL);
}
