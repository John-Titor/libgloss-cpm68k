
CTNG		 = /Volumes/CTNG/m68k-unknown-elf/
TOOL_PREFIX	 = $(CTNG)bin/m68k-unknown-elf-
ELF2CPM		 = ../Emulators/py68k/tools/elf2cpm.py

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
		   -Wno-pointer-sign
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
REL		 = build/$(PROG).rel

all: $(CPMLIB)

clean:
	rm -rf build

$(CPMLIB): $(OBJS)
	$(LD) $(LDFLAGS) -r -o $@ $(OBJS)

$(REL): $(ELF)
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

-include $(OBJS:.o=.d)

