#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "osif.h"
#include "osiferr.h"
#include "prototypes.h"

int
fstat(int fd , struct stat *buf)
{
    FD *fp;

    if ((fp = _chkc(fd)) == NULLFD)
        RETERR(FAILURE, EBADF);

    memset(buf, 0, sizeof(*buf));
    buf->st_mode = S_IREAD | S_IWRITE;
    buf->st_nlink = 1;

    if (fp->flags & (ISTTY | ISLPT | ISAUX)) {
        buf->st_mode |= S_IFCHR;
    } else {
        buf->st_mode |= S_IFREG;

        // _filesz moves the offset, so save it around the call...
        LONG curpos = fp->offset;
        buf->st_size = _filesz(fd);
        fp->offset = curpos;

        // XXX does not account for holes in the file...
        buf->st_blocks = (buf->st_size + SECSIZ - 1) / SECSIZ;
        buf->st_blksize = SECSIZ;
    }
    return 0;
}
