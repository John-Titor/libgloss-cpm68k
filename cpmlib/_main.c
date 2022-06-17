//
// CP/M pre-main code
//

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <newlib.h>

#include "prototypes.h"
#include "osif.h"

const BYTE * const __tname = "CON:";
const BYTE * const __lname = "LST:";

static void parse_args(char *tail, int *argcp, char ***argvp);
static void add_arg(char *arg, int *argcp, char ***argvp);
static void err(const char *msg);

// Check newlib configuration doesn't have any surprises.
//
#if HAVE_INITFINI_ARRAY != 1
# error Expected HAVE_INITFINI_ARARY to be 1 - need to call _init instead.
#endif
#if _WANT_REGISTER_FINI != 1
# error Expected _WANT_REGISTER_FINI to be 1 - need to manually atexit(_fini)
#endif
extern void __libc_init_array(void);
extern void __libc_fini_array(void);
void _init(void);
void _fini(void);

int
_main(uintptr_t basepage)
{
    // call initializer functions
    __libc_init_array();

    // parse command tail
    int argc = 0;
    char **argv = NULL;
    parse_args((char *)(basepage + 0x80), &argc, &argv);

    // call main and exit
    exit(main(argc, (const char **)argv, NULL));
}

void
_init(void)
{
   // do stdio init
    _chinit();                              /* Initialize channels      */
    _open(__tname,READ,0);                  /* Open STDIN               */
    _open(__tname,WRITE,0);                 /* Open STDOUT              */
    _open(__tname,WRITE,0);                 /* Open STDERR              */
}

void
_fini(void)
{
}

void
_exit(int code __unused)
{
    __OSIF(EXIT, 0);
    for (;;);
}

void
parse_args(char *tail, int *argcp, char ***argvp)
{
    add_arg("program", argcp, argvp);       // add bogus program name

    // get command tail length and guarantee the tail is nul-terminated
    int len = *tail++;
    if (len > 0x7e)
        len = 0x7e;
    char *cp = tail;
    *(cp + len) = '\0';

    while (*cp) {

        // skip whitespace
        if (isspace(*cp)) {
            cp++;
            continue;
        }

        // quoted string
        if ((*cp == '"') || (*cp == '\'')) {

            // find matching quote
            char *ep = strchr(cp + 1, *cp);
            if (!ep)
                err("unterminated quote$");

            // erase quote and add arg (may be empty)
            *ep = '\0';
            add_arg(cp + 1, argcp, argvp);

            // continue after quote
            cp = ep + 1;
            continue;
        }

        // argument
        char *ep = cp + 1;
        while (!isspace(*ep) && *ep)
            ep++;
        bool terminal = !*ep;
        *ep = '\0';
        add_arg(cp, argcp, argvp);

        // next arg or done
        cp = terminal ? ep : ep + 1;
    }
}

void
add_arg(char *arg, int *argcp, char ***argvp)
{
    *argvp = realloc(*argvp, ((*argcp) + 1) * sizeof(char *));
    (*argvp)[(*argcp)++] = arg;
}

void
err(const char *msg)
{
    __OSIF(C_WRITESTR, msg);
    exit(1);
}
