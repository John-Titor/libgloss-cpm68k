//
// CP/M-68K C library runtime support
//

#include <sys/cdefs.h>
#include <stdint.h>
#include <assert.h>

#pragma pack(push, 1)

struct CPMBasePage {
    uint32_t        tpaBase;
    uint32_t        tpaLimit;
    uint32_t        textStart;
    uint32_t        textLength;
    uint32_t        dataStart;
    uint32_t        dataLength;
    uint32_t        bssStart;
    uint32_t        bssLength;
    uint32_t        tpaFreeLength;
    uint8_t         loadDrive;
};

#pragma pack(pop)

// BDOS function numbers
enum {
	BDOS_Reset				= 0,
	BDOS_Console_Input		= 1,
	BDOS_Console_Output		= 2,
	BDOS_Direct_Console_IO	= 6,
	BDOS_Print_String		= 9,
	BDOS_Get_Console_Status	= 11,
};

// from crt0.S
extern struct CPMBasePage   *__cpmBasePage;

__BEGIN_DECLS

__END_DECLS
