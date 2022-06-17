// CP/M-68K C/C++ startup 
//

        .global _main
        .global _end

        // This is a gross hack due to the default linker script putting .text.exit before
        // everything else. The only right solution is going to be a custom linker script
        // for CP/M executables. *sigh*
        // Or the evil that the TOS converter does where it patches a jump into the head
        // of the binary and fudges all the relocations to suit...
        .section .text.unlikely

        // start(pBasePage)
        .global _start
_start:
        // no need to zero the BSS - BDOS has already done this for us 
        // no need to set up a stack - it's already at the end of the TPA
        // base page and return address are already on the stack

        suba.l          %a6, %a6                // clear the frame pointer
        jmp             _main                   // jump to next-level startup code
