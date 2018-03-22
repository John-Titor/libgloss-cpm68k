#include <sys/errno.h>
#include <unistd.h>

void *
sbrk(ptrdiff_t incr)
{
    extern char         _end;
    static char         *brk = &_end;
    register char       *stack __asm__("%sp");

    // make sure the heap doesn't smash the stack
    if ((stack < brk) || ((stack - brk) < 512)) {
        errno = ENOMEM;
        return (void *)-1;
    }

    void *ret = brk;
    brk += incr;

    return ret;
}
