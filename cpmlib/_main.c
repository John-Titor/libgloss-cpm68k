//
// CP/M pre-main code
//

#include <stdlib.h>

int _main(void);
extern int main(int argc, const char **argv, const char *envp);

int
_main(void)
{
    // XXX ctors
    // XXX argv / argc

    return main(0, NULL, NULL);
}
