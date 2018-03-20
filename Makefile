PROG		 = cpmtest

CTNG		 = /Volumes/CTNG/m68k-unknown-elf/
TOOL_PREFIX	 = $(CTNG)bin/m68k-unknown-elf-
ELF2CPM		 = ../Emulators/py68k/tools/elf2cpm.py

CC		 = $(TOOL_PREFIX)cc
CXX		 = $(TOOL_PREFIX)c++
LD		 = $(TOOL_PREFIX)cc

ARCHFLAGS	 = -m68000
OPTFLAGS	 = -g -O0 -ffunction-sections -fdata-sections
ASFLAGS		 = $(ARCHFLAGS)
CFLAGS		 = $(ARCHFLAGS) $(OPTFLAGS) -std=gnu11 -Wall
CXXFLAGS	 = $(ARCHFLAGS) $(OPTFLAGS) -std=gnu++11 -Wall
LDFLAGS		 = $(ARCHFLAGS) \
		   -nostartfiles \
		   -Wl,-static \
		   -Wl,-Map -Wl,$@.map \
		   -Wl,-emit-relocs \
		   -Wl,-Ttext=0 \
		   -Wl,-e_start \
		   -Wl,-gc-sections \
		   -Wl,-zmax-page-size=4 \
		   -Wl,-zcommon-page-size=4 \

SSRCS		 = $(wildcard *.S)
CSRCS		 = $(wildcard *.c)
CXXSRCS		 = $(wildcard *.cpp)


SOBJS		 = $(addprefix build/,$(SSRCS:.S=.o))
COBJS		 = $(addprefix build/,$(CSRCS:.c=.o))
CXXOBJS		 = $(addprefix build/,$(CXXSRCS:.cpp=.o))
OBJS		 = $(SOBJS) $(COBJS) $(CXXOBJS)

ELF		 = build/$(PROG).elf
REL		 = build/$(PROG).rel

all: $(REL)

clean:
	rm -rf build

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

