// CP/M-68K C/C++ startup 
//

	.global __main
	.global _exit

	// This is a gross hack due to the default linker script putting .text.exit before
	// everything else. The only right solution is going to be a custom linker script
	// for CP/M executables. *sigh*
	// Or the evil that the TOS converter does where it patches a jump into the head
	// of the binary and fudges all the relocations to suit...
	.section .text.unlikely
	.global _start
_start:
	move.l		4(%a7), %a0		// get the address of the basepage
	move.l		%a0, __cpmBasePage	// save for __main

	// no need to zero the BSS - BDOS has already done this for us 

	// no need to set up a stack - it's already at the end of the TPA

	// call main()
	suba.l		%a6, %a6		// clear the frame pointer
	jsr		__main			// call next-level startup code

	// main returned, exit
_exit:
	clr.l		%d0			// BDOS function 0
	trap		#2			// ... will exit

	.data
	.even

	.global __cpmBasePage
__cpmBasePage:
	.long		0
