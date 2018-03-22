
#include "portab.h"
#include "osif.h"

UWORD __BDOS(UWORD fn, ULONG arg)
{
  register UWORD function __asm__("%d0") = fn;
  register ULONG argument __asm__("%d1") = arg;
  register UWORD result __asm__("%d0");

  __asm__ volatile("trap #2" : "=r" (result) : "0" (function), "r" (argument) : "memory" );

  return result;
}
