
#pragma once

#include <sys/types.h>

#include "portab.h"
#include "osif.h"

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
extern WORD _creat(BYTE *fname, WORD prot, WORD type);
extern WORD creata(BYTE *fname, WORD prot);
extern WORD creatb(BYTE *fname, WORD prot);

// filesz.c
extern off_t _filesz(int fd);

// lstout.c
extern ssize_t _lstout(BYTE *buffer);

// __main.c
extern VOID __main(BYTE *com, WORD len);

// _main.c
extern BYTE *__tname;
extern BYTE *__lname;
extern int _main(void);

// __open.c
extern WORD __open(WORD ch, REG BYTE *filnam, WORD bdosfunc);

// open.c
extern WORD _open (BYTE *fname, WORD mode, WORD xtype);
extern WORD opena(BYTE *fname, WORD mode);
extern WORD openb(BYTE *fname, WORD mode);


// parsefn.c
extern BYTE *_parsefn(REG BYTE *filnam, struct fcbtab *fcbp);

// rdasc.c
extern ssize_t _rdasc(REG FD *fp, BYTE *buff, size_t bytes);

// rdbin.c
extern ssize_t _rdbin(REG FD *fp, BYTE *buff, size_t bytes);

// sbrk.c
extern void *brk(void *_newbrk);

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



