#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


int
fstat(int fd, struct stat *buf)
{
    errno = ENOSYS; // XXX TODO implement
    return -1;
}
