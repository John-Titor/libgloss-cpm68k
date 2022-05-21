
TOOL_PREFIX	 = m68k-elf-
ELF2CPM		 = build/elf2cpm68k
MAKE_DISK	 = ../Tiny68k/make_disk.sh
SIM		 = ../Emulators/py68k/py68k.py
SIM_ROM		 = ../Tiny68k/ROM/T68kbug_r07.BIN

# Compiler / linker
CC		 = $(TOOL_PREFIX)cc
CXX		 = $(TOOL_PREFIX)c++
LD		 = $(TOOL_PREFIX)cc

# Compilation / link options
ARCHFLAGS	 = -m68000
OPTFLAGS	 = -g \
		   -MMD \
		   -O2 \
		   -ffunction-sections \
		   -fdata-sections

WARNINGS	 = -Wall \
		   -Wextra \
		   -Wmisleading-indentation \
		   -Wstrict-prototypes \
		   -Wold-style-definition \
		   -Wmissing-parameter-type \
		   -Wmissing-prototypes \
		   -Wmissing-declarations \
		   -Wno-pointer-sign \
		   -Wno-char-subscripts

ASFLAGS		 = $(ARCHFLAGS)
CFLAGS		 = $(ARCHFLAGS) $(OPTFLAGS) -std=gnu11 $(WARNINGS)
CXXFLAGS	 = $(ARCHFLAGS) $(OPTFLAGS) -std=gnu++11 $(WARNINGS)
LDFLAGS		 = $(ARCHFLAGS) \
		   -nostartfiles \
		   -Wl,-static \
		   -Wl,-Map -Wl,$@.map \
		   -Wl,-emit-relocs \
		   -Wl,-Ttext=0 \
		   -Wl,-e_start \
		   -Wl,-gc-sections \
		   -Wl,-zmax-page-size=4 \
		   -Wl,-zcommon-page-size=4

# sources
SSRCS		 = $(wildcard *.S) $(wildcard cpmlib/*.S)
CSRCS		 = $(wildcard *.c) $(wildcard cpmlib/*.c)
CXXSRCS		 = $(wildcard *.cpp)

# derived objects
SOBJS		 = $(addprefix build/,$(SSRCS:.S=.o))
COBJS		 = $(addprefix build/,$(CSRCS:.c=.o))
CXXOBJS		 = $(addprefix build/,$(CXXSRCS:.cpp=.o))
OBJS		 = $(SOBJS) $(COBJS) $(CXXOBJS)

# the resulting library object
CPMLIB		 = build/cpmlib.o

ELF		 = build/$(PROG).elf
REL		 = build/$(PROG).68k
DMG		 = build/$(PROG).dmg

all: $(CPMLIB)

clean:
	rm -rf build

run: $(DMG)
	$(SIM) --target tiny68k --diskfile $(DMG) --eeprom $(SIM_ROM)

$(CPMLIB): $(OBJS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJS)

$(DMG): $(REL)
	$(MAKE_DISK) $@ $(REL)

$(REL): $(ELF) $(ELF2CPM)
	$(ELF2CPM) $< $@

$(ELF): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

$(SOBJS):build/%.o:%.S
	@mkdir -p $(dir $@)
	$(CC) $(ASLAGS) -c -o $@ $<

$(COBJS):build/%.o:%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(CXXOBJS):build/%.o:%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -c -o $@ $<

$(ELF2CPM): tools/elf2cpm68k.c
	cc -Wall $< -o $@

-include $(OBJS:.o=.d)

