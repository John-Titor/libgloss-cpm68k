********************************************************************
*
*       CP/M abort function for C Run Time Library
*
*       "abort" generates an Illegal Instruction Trap to recall DDT
*       (if it is loaded).
*
*********************************************************************

        .global abort
        .text
abort:
        move.w  %sp@(4),%d0
a:      .dc.w   0x4afc
        bra.b   a
