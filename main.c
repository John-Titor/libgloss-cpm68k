#include <stdio.h>
#include <unistd.h>
#include <string.h>

int 
main(int argc, const char **argv)
{
    printf("hello, CP/M-%dK from newlib\n", 68);
    printf("%d args\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argv[i]);
    }
    puts("name?");
    char s[128];
    gets(s);
    size_t n = strlen(s);
    if (n > sizeof(s)) {
        n = sizeof(s);
    }
    printf("len %lu\n", n);
    for (unsigned i = 0; i < n; i++) {
        printf("%u: %c 0x%02x\n", i, s[i], s[i]);
    }
    return 0;
}
