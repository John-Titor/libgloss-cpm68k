//
// CP/M-68K C library runtime support
//

#include <sys/stat.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "cpmLib.h"

extern int main(int argc, const char **argv, const char *envp);

static uint32_t BDOSCallArg(uint16_t bdosFunction, uintptr_t bdosParameter);

#define TRACE(_x)   BDOSCallArg(BDOS_Console_Output, _x)

void
__main()
{
    // XXX ctors
    // malloc?
    // XXX argv / argc

    exit(main(0, NULL, NULL));
}

static uint32_t
BDOSCallArg(uint16_t bdosFunction, uintptr_t bdosParameter)
{
    uint32_t result = 0;

    __asm__ volatile (
        "mov.w %1, %%d0\n\t"
        "mov.l %2, %%d1\n\t"
        "trap #2\n\t"
        "mov.l %%d0, %0"
        : "=r" (result)
        : "r" (bdosFunction), "r" (bdosParameter)
        : "d0", "d1", "memory"
        );

    return result;
}

static uint32_t
BDOSCallPtr(uint16_t bdosFunction, void *bdosParameter)
{
    return BDOSCallArg(bdosFunction, (uintptr_t)bdosParameter);
}

static uint32_t
BDOSCall(uint16_t bdosFunction)
{
    return BDOSCallArg(bdosFunction, 0);
}

//
// Newlib syscalls
//

void
__unimplemented(const char *what)
{
    for (int n = 0; n < 3; n++) {
        const char *cp;
        switch (n) {
            case 0:
                cp = "\nunimplemented: ";
                break;
            case 1:
                cp = what;
                break;
            case 3:
                cp = "\n";
                break;
        }
        while (*cp) {
            BDOSCallArg(BDOS_Console_Output, *cp++);
        }
    }
    _exit(1);
}

void *
sbrk(ptrdiff_t incr)
{
    static uintptr_t heapEnd = 0;

    if (heapEnd == 0) {
        heapEnd = __cpmBasePage->bssStart + __cpmBasePage->bssLength;
    }

    // XXX would be nice to check for heap/stack collision here
    void *ret = (void *)heapEnd;
    heapEnd += incr;

    return ret;
}

ssize_t
read(int fd, void *buf, size_t cnt)
{
    if (fd == STDIN_FILENO) {
        // Read at least one character from the console; more if they are
        // available.
        for (int i = 0; i < cnt; i++) {
            char c = BDOSCall(BDOS_Console_Input);
            // CP/M console / VT emulators send \r when Enter is pressed.
            // XXX need to re-consider this if we implement a 'raw' mode.
            if (c == '\r') {
                c = '\n';
            }

            *((char *)buf + i) = c;
            if (!BDOSCall(BDOS_Get_Console_Status)) {
                return i + 1;
            }
        }
        return cnt;
    } else if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        // can't read from stdout/stderr (well, shouldn't anyway)
        errno = EBADF;
        return -1;
    } else {
        __unimplemented("file read");
    }
    return 0;
}

ssize_t
write(int fd, const void *buf, size_t cnt)
{
    if (fd == STDIN_FILENO) {
        // can't write to stdin
        errno = EBADF;
        return -1;
    } else if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        // Write all characters to the console; may block as required.
        for (int i = 0; i < cnt; i++) {
            BDOSCallArg(BDOS_Console_Output, *((char *)buf + i));
        }
        return cnt;
    } else {
        __unimplemented("file write");
    }
    return 0;
}

int
close(int fd)
{
    printf("close %d\n", fd);
    return 0;
}

off_t
lseek(int fd, off_t pos, int whence)
{
    __unimplemented("lseek");
    return 0;
}

int
fstat(int fd, struct stat *pstat)
{
    memset(pstat, 0, sizeof(*pstat));

    if ((fd == STDIN_FILENO) ||
        (fd == STDOUT_FILENO) ||
        (fd == STDERR_FILENO)) {

        pstat->st_mode = S_IFCHR | S_IWUSR | S_IRUSR;
        pstat->st_blksize = 1;
    } else {
        __unimplemented("fstat");
    }
    return 0;
}

int
isatty(int fd)
{
    if ((fd == STDIN_FILENO) ||
        (fd == STDOUT_FILENO) ||
        (fd == STDERR_FILENO)) {
        return 1;
    }
    errno = ENOTTY;
    return 0;
}
