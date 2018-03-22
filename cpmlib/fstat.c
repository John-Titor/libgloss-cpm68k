#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


int
fstat(int fd __unused, struct stat *buf __unused)
{
    errno = ENOSYS; // XXX TODO implement
    return -1;
}
