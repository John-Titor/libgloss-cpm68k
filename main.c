#include <stdio.h>
#include <unistd.h>
#include <string.h>

int 
main(int argc, const char **argv)
{
    printf("hello, CP/M-%dK\n", 68);
    puts("name?");
    char s[128];
    gets(s);
    int n = strlen(s);
    if (n > sizeof(s)) {
        n = sizeof(s);
    }
    printf("len %d\n", n);
    for (unsigned i = 0; i < n; i++) {
        printf("%u: %c 0x%02x\n", i, s[i], s[i]);
    }
    return 0;
}
