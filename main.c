#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>

#define O_BINARY  0x10000   // XXX should be in a header

static void
_err(const char *msg)
{
    printf("ERROR: %s\n", msg);
    exit(1);
}

int 
main(int argc, const char **argv)
{
    printf("hello CP/M-%dK from newlib\n", 68);

    // args
    printf("%d args\n", argc);
    assert(argc >= 1);
    for (int i = 0; i < argc; i++) {
        assert(argv[i] != NULL);
        printf("%d: %s\n", i, argv[i]);
    }

    // open / read binary file
    {
        int fd = open("README.TXT", O_RDONLY | O_BINARY);
        printf("open: %d\n", fd);
        assert(fd >= 0);
        off_t count = 0;
        uint8_t c;
        while (read(fd, &c, 1) == 1) {
            count++;
        }
        assert(count > 0);

        // lseek
        off_t loc = lseek(fd, 0, SEEK_CUR);
        assert(loc == count);
        loc = lseek(fd, 0, SEEK_SET);
        assert(loc == 0);
        loc = lseek(fd, 0, SEEK_END);
        assert(loc == count);

        // fstat
        struct stat st;
        int ret = fstat(fd, &st);
        assert(ret == 0);
        assert(st.st_size == count);
        assert(st.st_mode & S_IFREG);

        // create new file
        int fd2 = open("README.TX2", O_RDWR | O_CREAT | O_BINARY);
        assert(fd2 >= 0);

        // copy old file to new in binary mode
        lseek(fd, 0, SEEK_SET);
        off_t count2 = 0;
        while (read(fd, &c, 1)) {
            int ret = write(fd2, &c, 1);
            assert(ret >= 0);
            count2++;
        }
        assert(count2 == lseek(fd2, 0, SEEK_CUR));
        assert(count2 == count);


        close(fd2);
        close(fd);
    }

    // open / read ascii file
    {
        FILE *fp = fopen("README.TXT", "r");
        if (!fp) {
            _err("failed to open README.TXT");
        }
        char buf[80];
        while (fgets(buf, sizeof(buf), fp)) {
            printf("README.TXT: %s\n", buf);
        }
    }

    // directory listing
    {
        // XXX no *dir functions
    }


    // puts("name?");
    // char s[128];
    // gets(s);
    // size_t n = strlen(s);
    // if (n > sizeof(s)) {
    //     n = sizeof(s);
    // }
    // printf("len %lu\n", n);
    // for (unsigned i = 0; i < n; i++) {
    //     printf("%u: %c 0x%02x\n", i, s[i], s[i]);
    // }

    puts("OK");
    return 0;
}
