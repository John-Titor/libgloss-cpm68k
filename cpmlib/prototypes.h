
#pragma once

#include <sys/types.h>

#include "portab.h"
#include "osif.h"
#include "sgtty.h"

extern int main(int argc, const char **argv, const char *envp);

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

// creat.c
extern WORD _creat(const BYTE *fname, WORD type);
extern WORD creata(const BYTE *fname);
extern WORD creatb(const BYTE *fname);

// filesz.c
extern off_t _filesz(int fd);

// lstout.c
extern ssize_t _lstout(BYTE *buffer);

// __main.c
extern void __main(BYTE *com, WORD len);

// _main.c
extern const BYTE * const __tname;
extern const BYTE * const __lname;
extern int _main(uintptr_t basepage);

// __open.c
extern WORD __open(WORD ch, const BYTE *filnam, WORD bdosfunc);

// open.c
extern WORD _open (const BYTE *fname, WORD mode, WORD xtype);
extern WORD opena(const BYTE *fname, WORD mode);
extern WORD openb(const BYTE *fname, WORD mode);


// parsefn.c
extern const BYTE *_parsefn(const BYTE *filnam, struct fcbtab *fcbp);

// rdasc.c
extern ssize_t _rdasc(FD *fp, BYTE *buff, size_t bytes);

// rdbin.c
extern ssize_t _rdbin(FD *fp, BYTE *buff, size_t bytes);

// sbrk.c
extern void *brk(void *_newbrk);

// sgtty.c
extern WORD stty(WORD fd, struct sgttyb *argp);
extern WORD gtty(WORD fd, struct sgttyb *argp);

// ttyin.c
extern ssize_t _ttyin(FD *fp, BYTE *buff, size_t bytes);

// ttyinraw.c
extern WORD ttyinraw(WORD chktype);

// ttyout.c
extern ssize_t _ttyout(BYTE *buf);

// wrtasc.c
extern ssize_t _wrtasc(FD *fp, const BYTE *buff, size_t bytes);

// wrtbin.c
extern ssize_t _wrtbin(FD *fp, const BYTE *buff, size_t bytes);

// wrtchr.c
extern ssize_t _wrtchr(FD *fp, const BYTE *buf, size_t bytes);



