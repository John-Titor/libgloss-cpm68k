#include <sys/errno.h>
#include <unistd.h>

#include "prototypes.h"

extern char         _end;
static char         *_brk = &_end;

void *
sbrk(ptrdiff_t incr)
{

    char *oldbrk = _brk;
    char *newbrk = _brk + incr;

    if (brk(newbrk) == (void *)-1) {
        errno = ENOMEM;
        return (void *)-1;
    }

    return oldbrk;
}

void *
brk(void *_newbrk)
{
    char *newbrk = (char *)_newbrk;

    // address must be even
    if ((uintptr_t)newbrk & 1) {
        newbrk++;
    }

    // keep a small buffer between heap and stack
    register char *stack __asm__("%sp");
    if ((stack < newbrk) || ((stack - newbrk) < 512)) {
        errno = ENOMEM;
        return (void *)-1;
    }

    _brk = newbrk;
    return newbrk;
}
