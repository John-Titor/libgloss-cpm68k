#include <sys/types.h>

#include "portab.h"

// internal prototypes

// blkio.c
extern LONG _blkio(FD *ccbp, LONG sector, BYTE *buffer, LONG count, int bdosfunc);

// channels.c
extern WORD _allocc(void);
extern WORD _freec(WORD ch);
extern void _chinit(void);
extern void __chinit(int i);
extern FD *_chkc(int ch);

// chkuser.c
extern WORD _chkuser(WORD newu);
extern void _uchkuser(WORD newu, WORD prevu);

// filesz.c
extern off_t _filesz(int fd);

// lstout.c
extern ssize_t _lstout(BYTE *buffer);

// rdasc.c
extern ssize_t _rdasc(REG FD *fp, BYTE *buff, size_t bytes);

// rdbin.c
extern ssize_t _rdbin(REG FD *fp, BYTE *buff, size_t bytes);

// ttyin.c
extern ssize_t _ttyin(FD *fp, BYTE *buff, size_t bytes);

// ttyout.c
extern ssize_t _ttyout(BYTE *buf);

// wrtasc.c
extern ssize_t _wrtasc(REG FD *fp, const BYTE *buff, size_t bytes);

// wrtbin.c
extern ssize_t _wrtbin(REG FD *fp, const BYTE *buff, size_t bytes);

// wrtchr.c
extern ssize_t _wrtchr(FD *fp, const BYTE *buf, size_t bytes);



